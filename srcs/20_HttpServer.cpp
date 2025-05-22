#include "20_HttpServer.hpp"

HttpServer::HttpServer(const Config& config) : _config(config)
{
}

void    HttpServer::initialize()
{
    setupServerSockets();
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
        
        std::cout << "Server listening on port " << port << "std::endl";
    }
    return (0);
}

void    HttpServer::run()
{
    int epollFd = epoll_create(1);
    if (epollFd < 0)
    {
        throw FailedSocket();
    }

    for (std::map<int, int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); ++it)
    {
        int serverFd = it->second;
        struct epoll_event  ev;
        std::memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN;
        ev.data.fd = serverFd;

        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev) < 0)
        {
            throw FailedSocket();
        }
    }

    const int   MAX_EVENTS = 64;
    struct epoll_event  events[MAX_EVENTS];
    
    while (1)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);

        if (numEvents < 0)
        {
            if (errno == EINTR)
                continue;
            perror("epoll_wait");
            break ;
        }

        for (int i = 0; i < numEvents; i++)
        {
            int currentFd = events[i].data.fd;

            // 새연결
            std::map<int, int>::iterator server_it = _serverSockets.find(currentFd);
            if (server_it != _serverSockets.end())
            {
                acceptNewConnection(currentFd, epollFd);
            }
            //  클라 소켓
            else
            {
                if (events[i].events & EPOLLIN)
                {
                    int res = handleClientRequest(currentFd);

                    //  처리 후
                    std::map<int, ClientData>::iterator client_it = _clients.find(currentFd);
                    if (client_it == _clients.end())
                        continue;
                    if (res < 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK)
                        {
                            std::cerr << "Error handling client request: ";
                            std::cerr << strerror(errno) << std::endl;
                            closeClientConnection(currentFd, epollFd);
                        }
                        continue;
                    }
                    // 쓰기 이벤트 등록
                    if (client_it->second.responseReady)
                    {
                        struct epoll_event  ev;
                        std::memset(&ev, 0, sizeof(ev));
                        ev.events = EPOLLIN;
                        ev.data.fd = currentFd;
                        epoll_ctl(epollFd, EPOLL_CTL_ADD, currentFd, &ev);
                    }
                }
                //  응답 전송
                else if (events[i].events & EPOLLOUT)
                {
                    int res = sendResponse(currentFd);

                    // 전송 후
                    std::map<int, ClientData>::iterator client_it = _clients.find(currentFd);

                    if (client_it == _clients.end())
                        continue;

                    if (res < 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK)
                        {
                            std::cerr << "Error handling client request: ";
                            std::cerr << strerror(errno) << std::endl;
                            closeClientConnection(currentFd, epollFd);
                        }
                        continue;
                    }

                    if (!client_it->second.responseReady)
                    {
                        closeClientConnection(currentFd, epollFd);
                    }
                }
                if (events[i].events & (EPOLLERR | EPOLLHUP))
                {
                    std::cerr << "Error or hangup on client socket (fd: " << currentFd << ")" << std::endl;
                    closeClientConnection(currentFd, epollFd);
                }
            }
        }
    }
    close(epollFd);
}

void HttpServer::acceptNewConnection(int serverFd, int epollFd) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientFd < 0) {
        perror("Failed to accept connection");
        return;
    }
    
    // 비차단 모드 설정
    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags < 0 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("Failed to set non-blocking mode for client");
        close(clientFd);
        return;
    }
    
    // 클라이언트 정보 출력
    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
    std::cout << "New client connected: " << clientIp << ":" 
              << ntohs(clientAddr.sin_port) << " (fd: " << clientFd << ")" << std::endl;
    
    // 클라이언트 데이터 초기화
    ClientData clientData;
    clientData.socketFd = clientFd;
    clientData.requestComplete = false;
    clientData.responseReady = false;
    clientData.server = NULL;
    clientData.location = NULL;
    _clients[clientFd] = clientData;
    
    // epoll에 클라이언트 소켓 등록 (읽기 이벤트)
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;  // 옵션: EPOLLET 추가 가능
    ev.data.fd = clientFd;
    
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0) {
        perror("Failed to add client socket to epoll");
        _clients.erase(clientFd);
        close(clientFd);
    }
}

void HttpServer::closeClientConnection(int clientFd, int epollFd) {
    std::map<int, ClientData>::iterator it = _clients.find(clientFd);
    if (it != _clients.end()) {
        std::cout << "Closing client connection (fd: " << clientFd << ")" << std::endl;
        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
        close(clientFd);
        _clients.erase(it);
    }
}

int HttpServer::handleClientRequest(int clientFd)
{
    clientFd = 0;
    return (1);
}

void    HttpServer::processRequset(ClientData& client)
{
    (void)client;
}

void    HttpServer::buildResponse(ClientData& client)
{
    (void)client;
}

int HttpServer::sendResponse(int clientFd)
{
    clientFd = 0;
    return (1);
}

const char *HttpServer::FailedSocket::what() const throw()
{
    return ("Here's Johnny!");
}

HttpServer::~HttpServer()
{

}