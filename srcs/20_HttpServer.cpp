#include "20_HttpServer.hpp"

HttpServer::HttpServer(const Config& config) : _config(config)
{
    _epollFd = -1;
}


void    HttpServer::initialize()
{
    setupServerSockets();
    initaliseEpoll(&_epollFd);
}


int    HttpServer::setupServerSockets()
{
    const   std::vector<Server>& servers = _config.getServers();

    for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
    {
        const Server& server = *it;
        int port = server.getPort();

        if (_serverSockets.find(port) != _serverSockets.end())
            continue;

        int serverFd = socket(PF_INET, SOCK_STREAM, 0);
        if (serverFd < 0)
        {
            throw   FailedSocket();
        }
        int opt = 1;
        if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            close(serverFd);
            throw   FailedSocket();
        }

        int flags = fcntl(serverFd, F_GETFL, 0);
        if (flags == -1)
        {
            close(serverFd);
            throw   FailedSocket();
        }
        if (fcntl(serverFd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            close(serverFd);
            throw   FailedSocket();
        }
        struct sockaddr_in  socketAddr;
        std::memset(&socketAddr, 0, sizeof(socketAddr));
        socketAddr.sin_family = AF_INET;
        socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        socketAddr.sin_port = htons(port);

        if (bind(serverFd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) < 0)
        {
            close(serverFd);
            throw   FailedSocket();
        }

        if (listen(serverFd, 10) < 0)
        {
            close(serverFd);
            throw   FailedSocket();
        }
        _serverSockets[port] = serverFd;
        // FD_SET(serverFd, &_readFds);

        // if (serverFd > _maxFd)
        //     _maxFd = serverFd;
        
        std::cout << "Server listening on port " << port << std::endl;
    }
    return (0);
}

void    HttpServer::initaliseEpoll(int *epollFd)
{
    struct epoll_event  ev;

    *epollFd = epoll_create(1);   
    if (*epollFd < 0)
        throw FailedSocket();
    
    for (std::map<int, int>::iterator it = _serverSockets.begin(); \
            it != _serverSockets.end(); ++it)
    {
        std::memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN;
        ev.data.fd = it->second; // server Fd
        if (epoll_ctl(*epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0)
        {
            throw FailedSocket();
        }
    }
}


bool    HttpServer::isServerSocket(int currentFd)
{
    bool isServerSocket = false;
    for (std::map<int, int>::iterator it = _serverSockets.begin(); 
            it != _serverSockets.end(); ++it)
    {
        if (it->second == currentFd) 
        {
            isServerSocket = true;
            break;
        }
    }
    return isServerSocket;
}

void    HttpServer::run()
{
    int                 numEvents;
    struct epoll_event  events[MAX_EVENTS];
    std::cout << "=== HttpServer::run() started ===" << std::endl;

    while (1)
    {
        std::cout << "Waiting for events..." << std::endl;
        numEvents = epoll_wait(_epollFd, events, MAX_EVENTS, -1);        
        if (numEvents < 0)
        {
            if (errno == EINTR)
                continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < numEvents; i++)
        {
            int currentFd = events[i].data.fd;                     
            if (isServerSocket(currentFd)) // Server Socket
                acceptNewConnection(currentFd, _epollFd);
            else // Client Socket
            {
                if (events[i].events & EPOLLIN)
                {
                    if (handleClientRead(currentFd))
                        continue;
                }
                else if (events[i].events & EPOLLOUT)
                {
                    if (handleClientWrite(currentFd))
                        continue;
                }
                else if (events[i].events & (EPOLLERR | EPOLLHUP))
                {
                    std::cerr << "Error or hangup on client socket (fd: " << currentFd << ")" << std::endl;
                    closeClientConnection(currentFd,_epollFd);
                }
            }
        }
    }
    close(_epollFd);
}


// void    HttpServer::acceptNewConnection(int serverFd, int epollFd) {
//     int                 serverPort;
//     int                 clientFd;
//     struct sockaddr_in  clientAddr;
//     socklen_t           clientAddrLen;
    
//     clientAddrLen = sizeof(clientAddr);
//     clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
//     if (clientFd < 0)
//     {
//         std::cerr << "Failed to accept connection" << std::endl;
//         return ;
//     }
    
//     if (!setupClientSocket(clientFd))
//     {
//         close(clientFd);
//         return ;
//     }
    
//     serverPort = getServerPort(serverFd);

//     logNewConnection(clientAddr, clientFd);
    
//     ClientData client(clientFd);
//     client.setServerPort(serverPort);
//     // _clients[clientFd] = client;
//     _clients.insert(std::make_pair(clientFd, client));
//     if (!registerClientToEpoll(clientFd, epollFd))
//     {
//         _clients.erase(clientFd);
//         close(clientFd);
//         return;
//     }
// }

void HttpServer::acceptNewConnection(int serverFd, int epollFd) 
{
    int serverPort;
    int clientFd;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;
    
    clientAddrLen = sizeof(clientAddr);
    clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd < 0)
    {
        std::cerr << "Failed to accept connection" << std::endl;
        return;
    }
    
    std::cout << "Accepted connection, clientFd: " << clientFd << std::endl;
    
    if (!setupClientSocket(clientFd))
    {
        std::cout << "Failed to setup client socket" << std::endl;
        close(clientFd);
        return;
    }
    
    serverPort = getServerPort(serverFd);
    std::cout << "Server port: " << serverPort << std::endl;

    logNewConnection(clientAddr, clientFd);
    
    ClientData client(clientFd);
    client.setServerPort(serverPort);
    
    // 클라이언트를 맵에 추가
    // std::pair<std::map<int, ClientData>::iterator, bool> result = 
    _clients[clientFd] = client;
    
    // if (!result.second)
    // {
    //     std::cerr << "Failed to insert client into map!" << std::endl;
    //     close(clientFd);
    //     return;
    // }
    
    std::cout << "Client added to map, current map size: " << _clients.size() << std::endl;
    
    if (!registerClientToEpoll(clientFd, epollFd))
    {
        std::cout << "Failed to register client to epoll" << std::endl;
        _clients.erase(clientFd);
        close(clientFd);
        return;
    }
    
    std::cout << "Client successfully registered" << std::endl;
}

int     HttpServer::getServerPort(int serverFd)
{
    int port;

    port = 1;
    for (std::map<int, int>::iterator it = _serverSockets.begin(); \
        it != _serverSockets.end(); ++it)
    {
        if (it->second == serverFd) 
        {
            port = it->first;
            break;
        }
    }
    return (port);
}

bool    HttpServer::setupClientSocket(int clientFd)
{
    int flags;

    flags = fcntl(clientFd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << "Failed to get socket flags" << std::endl;
        return (false);
    }
    
    if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        std::cerr << "Failed to set non-blocking mode for client" << std::endl;
        return (false);
    }
    
    return (true);
}

void    HttpServer::logNewConnection(const struct sockaddr_in& clientAddr, int clientFd)
{
    char clientIp[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
    std::cout << "New client connected: " << clientIp << ":" 
              << ntohs(clientAddr.sin_port) << " (fd: " << clientFd << ")" 
              << std::endl;
}

bool    HttpServer::registerClientToEpoll(int clientFd, int epollFd)
{
    struct epoll_event ev;
    
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = clientFd;
    
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0)
    {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        return (false);
    }
    return (true);
}

int HttpServer::handleClientRead(int currentFd)
{
    struct epoll_event ev;
    
    std::cout << "=== handleClientRead for fd: " << currentFd << " ===" << std::endl;
    
    // 클라이언트가 맵에 있는지 먼저 확인
    std::map<int, ClientData>::iterator client_it = _clients.find(currentFd);
    if (client_it == _clients.end())
    {
        std::cerr << "Client not found in map for fd: " << currentFd << std::endl;
        closeClientConnection(currentFd, _epollFd);
        return 1;
    }
    
    try
    {
        std::cout << "Creating HttpRequest..." << std::endl;
        client_it->second.setReq(new HttpRequest(currentFd));
        
        std::cout << "Creating HttpResponse..." << std::endl;
        client_it->second.setResp(new HttpResponse(*(client_it->second.getReq())));
        
        std::cout << "Setting server..." << std::endl;
        client_it->second.setServer(this->_config);
        
        std::cout << "Setting location..." << std::endl;
        client_it->second.setLocation();
        
        std::cout << "Creating response..." << std::endl;
        client_it->second.createResponse();

        if (client_it->second.getRespReady() == true)
        {
            std::cout << "Response ready, switching to EPOLLOUT" << std::endl;
            std::memset(&ev, 0, sizeof(ev));
            ev.events = EPOLLOUT;
            ev.data.fd = currentFd;
            epoll_ctl(_epollFd, EPOLL_CTL_MOD, currentFd, &ev);
        }
        else
        {
            std::cout << "Response not ready!" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request processing error: " << e.what() << std::endl;
        closeClientConnection(currentFd, _epollFd);
        return 1;
    }
    
    return 0;
}
int     HttpServer::handleClientWrite(int currentFd)
{
    int                                 res;
    std::map<int, ClientData>::iterator client_it;
    
    client_it = _clients.find(currentFd);
    if (client_it == _clients.end())
        return (1);
    
    res = sendResponse(currentFd);
    if (res < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            std::cerr << "Error sending response: " << strerror(errno) << std::endl;
            closeClientConnection(currentFd,_epollFd);
        }
        return (1);
    }

    closeClientConnection(currentFd,_epollFd);
    return (0);
}

void    HttpServer::closeClientConnection(int clientFd, int epollFd)
{
    std::map<int, ClientData>::iterator it = _clients.find(clientFd);

    if (it != _clients.end()) {
        std::cout << "Closing client connection (fd: " << clientFd << ")" << std::endl;
        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
        close(clientFd);
        _clients.erase(it);
    }
}

std::string HttpServer::extractPath(const std::string& url)
{
    size_t  queryPos = url.find('?');
    if (queryPos != std::string::npos)
        return (url.substr(0, queryPos));
    return (url);
}

std::string HttpServer::buildFilePath(const Location* location, const std::string& path)
{
    std::string root = location ? location->getRoot() : "./www";
    if (!root.empty() && root[root.length() - 1] == '/')
        root = root.substr(0, root.length() - 1);
    return root + path;
}

bool        HttpServer::isMethodAllowd(const Location* location, HttpMethod method)
{
    (void)method;
    if (!location)
        return true;
    return true;
}

void    HttpServer::buildResponse(ClientData& client)
{
    (void)client;
}

int HttpServer::sendResponse(int clientFd)
{
    std::cout << "=== sendResponse called for fd: " << clientFd << " ===" << std::endl;
    std::map<int, ClientData>::iterator it = _clients.find(clientFd);
    if (it == _clients.end())
    {
        std::cout << "Client not found in map" << std::endl;
        return -1;
    }
    
    // std::cout << "Client found, responseReady: " << it->second.responseReady << std::endl;  
    
    if (!it->second.getRespReady())
    {
        std::cout << "Response not ready" << std::endl;
        return -1;
    }
    
    const std::string& response = it->second.getResp()->getTotalResp();
    std::cout << "Response size: " << response.length() << " bytes" << std::endl;  
    
    ssize_t sent = send(clientFd, response.c_str(), response.length(), 0);
    
    if (sent < 0)
    {
        perror("send failed");  // 구체적인 에러 확인
        return -1;
    }
    
    std::cout << "Response sent: " << sent << " bytes" << std::endl;
    return sent;
}

HttpServer::~HttpServer()
{
}

// ClientData& HttpServer::getClientData(int currentFd)
// {
//     std::map<int, ClientData>::iterator client_it;

//     client_it = _clients.find(currentFd);
    
//     if (client_it == _clients.end())
//         throw InvalidCurrentFd();
    
//     return (client_it->second);
// }

ClientData& HttpServer::getClientData(int currentFd)
{
    std::cout << "Looking for client fd: " << currentFd << " in map of size: " << _clients.size() << std::endl;
    
    std::map<int, ClientData>::iterator client_it = _clients.find(currentFd);
    
    if (client_it == _clients.end())
    {
        std::cout << "Available fds in map: ";
        for (std::map<int, ClientData>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
        
        throw InvalidCurrentFd();
    }
    
    return client_it->second;
}

const char *HttpServer::FailedSocket::what() const throw()
{
    return ("Here's Johnny!");
}

const char *HttpServer::InvalidCurrentFd::what() const throw()
{
    return ("Here's Johnny!");
}