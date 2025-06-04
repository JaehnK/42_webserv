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


void    HttpServer::acceptNewConnection(int serverFd, int epollFd) {
    int                 serverPort;
    int                 clientFd;
    struct sockaddr_in  clientAddr;
    socklen_t           clientAddrLen;
    
    clientAddrLen = sizeof(clientAddr);
    clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd < 0)
    {
        std::cerr << "Failed to accept connection" << std::endl;
        return ;
    }
    
    if (!setupClientSocket(clientFd))
    {
        close(clientFd);
        return ;
    }
    
    serverPort = getServerPort(serverFd);

    logNewConnection(clientAddr, clientFd);
    
    ClientData client(clientFd);
    client.setServerPort(serverPort);
    // _clients[clientFd] = client;
    _clients.insert(std::make_pair(clientFd, client));
    if (!registerClientToEpoll(clientFd, epollFd))
    {
        _clients.erase(clientFd);
        close(clientFd);
        return;
    }
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


int     HttpServer::handleClientRead(int currentFd)
{
    
    struct epoll_event      ev;

    ClientData client = getClientData(currentFd);
    try
    {
        
        client.setReq(new HttpRequest(currentFd));
        client.setResp(new HttpResponse(*(client.getReq())));
        client.setServer(this->_config);
        client.setLocation();
        // processRequest(client->second);

        client.createResponse();

        if (client.getRespReady() == true)
        {
            std::memset(&ev, 0, sizeof(ev));
            ev.events = EPOLLOUT;
            ev.data.fd = currentFd;
            epoll_ctl(_epollFd, EPOLL_CTL_MOD, currentFd, &ev);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request processing error: " << e.what() << std::endl;
        closeClientConnection(currentFd, _epollFd);
        return (1);
    }
    return (0);
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

// void    HttpServer::processRequest(ClientData& client)
// {
//     try
//     {
//         HttpRequest* req = client.parseRequest;

//         //  req loging
//         // std::cout << "Processing: " << getMethodString(req->getMethod()) << " " << req->getUrl() << std::endl;

//         //  server matching
//         std::string host = req->getHeaders().count("Host") ? req->getHeaders().at("Host") : "localhost";
//         client.server = findMatchingServer(host, client.socketFd);
//         std::cout << "About to find matching location..." << std::endl;
//         //  Location matching
//         std::string path = extractPath(req->getUrl());
//         client.location = findMatchingLocation(client.server, path);
//         std::cout << "Location found: " << (client.location ? "YES" : "NO") << std::endl;
//         std::cout << "client.location pointer: " << client.location << std::endl;
//         if (client.location) {
//             std::cout << "About to access location methods..." << std::endl;
            
//             // isMethodAllowed 호출 전에 디버그 추가
//             std::cout << "About to call isMethodAllowed..." << std::endl;
//         }
//         if (!isMethodAllowd(client._location, req->getMethod()))
//         {
//             std::cout << "Method not allowed" << std::endl;
//             client.response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 18\r\n\r\nMethod Not Allowed";
//             client.responseReady = true;
//             return ;
//         }
//         std::cout << "Method is allowed, proceeding..." << std::endl;

//         // 메서드별 처리
//         std::cout << "About to enter switch statement..." << std::endl;
//         switch (req->getMethod())
//         {
//             case METHOD_GET:
//                 std::cout << "Calling handleGetRequest..." << std::endl;
//                 handleGetRequest(client);
//                 std::cout << "handleGetRequest completed" << std::endl;
//                 break;
//             case METHOD_POST:
//                 std::cout << "Calling handlePostRequest..." << std::endl;
//                 handlePostRequest(client);
//                 break;
//             case METHOD_DELETE:
//                 std::cout << "Calling handleDeleteRequest..." << std::endl;
//                 handleDeleteRequest(client);
//                 break;
//             default:
//                 std::cout << "Unknown method, sending 501..." << std::endl;
//                 client.response = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 15\r\n\r\nNot Implemented";
//                 break;
//         }
        
//         std::cout << "About to set responseReady..." << std::endl;
//         client.responseReady = true;
//         std::cout << "Request processed successfully" << std::endl;
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << "Error processing request: " << e.what() << std::endl;
//         client.response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\n\r\nInternal Server Error";
//         client.responseReady = true;
//     }
    
// }


// void    HttpServer::handleGetRequest(ClientData& client)
// {
//     std::cout << "=== handleGetRequest START ===" << std::endl;
    
//     // std::cout << "client.parseRequest pointer: " << client.parseRequest << std::endl;
//     if (!client.parseRequest) {
//         std::cout << "ERROR: client.parseRequest is NULL!" << std::endl;
//         return;
//     }
//     HttpRequest* req = client.parseRequest;
//     std::cout << "Got HttpRequest pointer: " << req << std::endl;
    
//     std::cout << "About to call extractPath..." << std::endl;
//     std::string path = extractPath(req->getUrl());
//     std::cout << "Extracted path: [" << path << "]" << std::endl;

//     if (path == "/")
//         path = "/index.html";
//     std::cout << "Final path: [" << path << "]" << std::endl;
//     // std::string filePath = buildFilePath(client.location, path);
//     // std::ifstream file(filePath.c_str());
//     std::cout << "About to call buildFilePath..." << std::endl;
//     std::cout << "client.location pointer: " << client.location << std::endl;
    
//     // buildFilePath 대신 직접 경로 설정으로 테스트
//     std::string filePath = "./www" + path;
//     std::cout << "Using direct file path: [" << filePath << "]" << std::endl;
    
//     std::cout << "About to open file..." << std::endl;
//     std::ifstream file(filePath.c_str());

//     if (file.good())
//     {
//         std::cout << "File opened successfully" << std::endl;
//         std::ostringstream buffer;
//         buffer << file.rdbuf();
//         std::string content = buffer.str();
//         file.close();
//         std::cout << "File read successfully, size: " << content.length() << std::endl;
//         std::string contentType = "text/html";  // 기본값
//         size_t dotPos = filePath.find_last_of('.');
//         if (dotPos != std::string::npos) {
//             std::string extension = filePath.substr(dotPos);
//             if (extension == ".css")
//                 contentType = "text/css";
//             else if (extension == ".js")
//                 contentType = "application/javascript";
//             else if (extension == ".html" || extension == ".htm")
//                 contentType = "text/html";
//             else if (extension == ".png")
//                 contentType = "image/png";
//             else if (extension == ".jpg" || extension == ".jpeg")
//                 contentType = "image/jpeg";
//         }
//         std::ostringstream response;
//         response << "HTTP/1.1 200 OK\r\n";
//         response << "Content-Type: " << contentType << "\r\n";  // 동적 Content-Type
//         response << "Content-Length: " << content.length() << "\r\n";
//         response << "\r\n";
//         response << content;

//         client.response = response.str();
//         std::cout << "Response prepared, size: " << client.response.length() << std::endl;
//         std::cout << "Serving file: " << filePath << std::endl;
//     }
//     else
//     {
//         std::cout << "File not found: " << filePath << std::endl;
//         std::string notFound = "<!DOCTYPE html><html><body><h1>404 Not Found</h1></body></html>";
//         std::ostringstream  response;
//         response << "HTTP/1.1 404 Not Found\r\n";
//         response << "Content-Type: text/html\r\n";
//         response << "Content-Length: " << notFound.length() << "\r\n";
//         response << "\r\n";
//         response << notFound;

//         client.response = response.str();
//         // std::cout << "File not found: " << filePath << std::endl;
//     }
//     std::cout << "=== handleGetRequest END ===" << std::endl;
// }

// void    HttpServer::handlePostRequest(ClientData& client)
// {
//     HttpRequest* req = client.parseRequest;
//     std::string body = req->getBody();

//     std::cout << "POST data received: " << body.length() << " bytes" << std::endl;

//     std::string responseBody = "POST data received successfully";
//     std::ostringstream  response;
//     response << "HTTP/1.1 200 OK\r\n";
//     response << "Content-Type: text/plain\r\n";
//     response << "Content-Length: " << responseBody.length() << "\r\n";
//     response << "\r\n";
//     response << responseBody;

//     client.response = response.str();
// }

// void    HttpServer::handleDeleteRequest(ClientData& client)
// {
//     HttpRequest* req = client.parseRequest;
//     std::string path = extractPath(req->getUrl());

//     std::cout << "DELETE request for: " << path << std::endl;

//     std::string responseBody = "DELETE request processed";
//     std::ostringstream response;
//     response << "HTTP/1.1 200 OK\r\n";
//     response << "Content-Type: text/plain\r\n";
//     response << "Content-Length: " << responseBody.length() << "\r\n";
//     response << "\r\n";
//     response << responseBody;

//     client.response = response.str();
// }

// void    HttpServer::handleCgiRequest(ClientData& client, LocationCGI* cgiLocation)
// {
//     (void)cgiLocation;
//     client.response = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 15\r\n\r\nCGI Not Ready";
// }

// std::string HttpServer::getMethodString(HttpMethod method)
// {
//     switch (method)
//     {
//         case METHOD_GET: return "GET";
//         case METHOD_POST: return "POST";
//         case METHOD_DELETE: return "DELETE";
//         default: return "UNKNOWN";
//     }
// }

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

ClientData& HttpServer::getClientData(int currentFd)
{
    std::map<int, ClientData>::iterator client_it;

    client_it = _clients.find(currentFd);
    
    if (client_it == _clients.end())
        throw InvalidCurrentFd();
    
    return (client_it->second);
}

const char *HttpServer::FailedSocket::what() const throw()
{
    return ("Here's Johnny!");
}

const char *HttpServer::InvalidCurrentFd::what() const throw()
{
    return ("Here's Johnny!");
}