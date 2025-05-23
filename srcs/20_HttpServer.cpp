#include "20_HttpServer.hpp"

HttpServer::HttpServer(const Config& config) : _config(config), _epollFd(-1)
{
    (void)_epollFd;  // 미사용 경고 제거
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
        
        std::cout << "Server listening on port " << port << std::endl;
    }
    return (0);
}

void    HttpServer::run()
{
    std::cout << "=== HttpServer::run() started ===" << std::endl;
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
        std::cout << "Waiting for events..." << std::endl;
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        std::cout << "Got " << numEvents << " events" << std::endl;
        
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
            std::cout << "Processing event for fd: " << currentFd << std::endl;

            // 서버 소켓인지 확인 (수정된 부분)
            bool isServerSocket = false;
            for (std::map<int, int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); ++it)
            {
                if (it->second == currentFd) {
                    isServerSocket = true;
                    break;
                }
            }

            // 새 연결
            if (isServerSocket)
            {
                std::cout << "SERVER SOCKET FOUND! Calling acceptNewConnection" << std::endl;
                acceptNewConnection(currentFd, epollFd);
            }
            // 클라이언트 소켓
            else
            {
                std::cout << "Client socket event detected" << std::endl;
                
                if (events[i].events & EPOLLIN)
                {
                    std::cout << "EPOLLIN event detected" << std::endl;  // 추가
                    std::map<int, ClientData>::iterator client_it = _clients.find(currentFd);
                    if (client_it == _clients.end())
                        continue;
                    std::cout << "Client found in map" << std::endl;  // 추가
                    try
                    {
                        std::cout << "Creating HttpRequest..." << std::endl;  // 추가
                        // HttpRequest 객체 생성 및 할당 (수정된 부분)
                        client_it->second.parseRequest = new HttpRequest(currentFd);
                        std::cout << "HttpRequest created successfully" << std::endl;  // 추가
        
                        std::cout << "Calling processRequest..." << std::endl;  // 추가
                        processRequest(client_it->second);
                        std::cout << "processRequest completed" << std::endl;  // 추가
                        // 응답 준비되면 쓰기 이벤트로 변경 (수정된 부분)
                        if (client_it->second.responseReady)
                        {
                            struct epoll_event  ev;
                            std::memset(&ev, 0, sizeof(ev));
                            ev.events = EPOLLOUT;  // EPOLLIN → EPOLLOUT 수정
                            ev.data.fd = currentFd;
                            epoll_ctl(epollFd, EPOLL_CTL_MOD, currentFd, &ev);  // ADD → MOD 수정
                        }
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << "Request processing error: " << e.what() << std::endl;
                        closeClientConnection(currentFd, epollFd);
                    }
                }
                // 응답 전송
                else if (events[i].events & EPOLLOUT)
                {
                    std::map<int, ClientData>::iterator client_it = _clients.find(currentFd);
                    if (client_it == _clients.end())
                        continue;

                    int res = sendResponse(currentFd);

                    if (res < 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK)
                        {
                            std::cerr << "Error sending response: " << strerror(errno) << std::endl;
                            closeClientConnection(currentFd, epollFd);
                        }
                        continue;
                    }

                    // 응답 전송 완료 후 연결 종료
                    closeClientConnection(currentFd, epollFd);
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
    clientData.parseRequest = NULL;
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

void HttpServer::closeClientConnection(int clientFd, int epollFd)
{
    std::map<int, ClientData>::iterator it = _clients.find(clientFd);

    if (it != _clients.end()) {
        std::cout << "Closing client connection (fd: " << clientFd << ")" << std::endl;
        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
        close(clientFd);
        _clients.erase(it);
    }
}

void    HttpServer::processRequest(ClientData& client)
{
    if (!client.parseRequest)
    {
        throw   FailedSocket();
    }
    try
    {
        HttpRequest* req = client.parseRequest;

        //  req loging
        std::cout << "Processing: " << getMethodString(req->getMethod()) << " " << req->getUrl() << std::endl;

        //  server matching
        std::string host = req->getHeaders().count("Host") ? req->getHeaders().at("Host") : "localhost";
        client.server = findMatchingServer(host, client.socketFd);
        std::cout << "About to find matching location..." << std::endl;
        //  Location matching
        std::string path = extractPath(req->getUrl());
        client.location = findMatchingLocation(client.server, path);
        std::cout << "Location found: " << (client.location ? "YES" : "NO") << std::endl;
        std::cout << "client.location pointer: " << client.location << std::endl;
        if (client.location) {
            std::cout << "About to access location methods..." << std::endl;
            
            // isMethodAllowed 호출 전에 디버그 추가
            std::cout << "About to call isMethodAllowed..." << std::endl;
        }
        if (!isMethodAllowd(client.location, req->getMethod()))
        {
            std::cout << "Method not allowed" << std::endl;
            client.response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 18\r\n\r\nMethod Not Allowed";
            client.responseReady = true;
            return ;
        }
        std::cout << "Method is allowed, proceeding..." << std::endl;

        // 메서드별 처리
        std::cout << "About to enter switch statement..." << std::endl;
        switch (req->getMethod())
        {
            case METHOD_GET:
                std::cout << "Calling handleGetRequest..." << std::endl;
                handleGetRequest(client);
                std::cout << "handleGetRequest completed" << std::endl;
                break;
            case METHOD_POST:
                std::cout << "Calling handlePostRequest..." << std::endl;
                handlePostRequest(client);
                break;
            case METHOD_DELETE:
                std::cout << "Calling handleDeleteRequest..." << std::endl;
                handleDeleteRequest(client);
                break;
            default:
                std::cout << "Unknown method, sending 501..." << std::endl;
                client.response = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 15\r\n\r\nNot Implemented";
                break;
        }
        
        std::cout << "About to set responseReady..." << std::endl;
        client.responseReady = true;
        std::cout << "Request processed successfully" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Erorr processing request: " << e.what() << std::endl;
        client.response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\n\r\nInternal Server Error";
        client.responseReady = true;
    }
    
}

Server* HttpServer::findMatchingServer(const std::string& host, int port)
{
    (void)host;
    (void)port;
    const std::vector<Server>& servers = _config.getServers();

    return servers.empty() ? NULL : const_cast<Server*>(&servers[0]);
}

Location* HttpServer::findMatchingLocation(Server* server, const std::string& path)
{
    std::cout << "=== findMatchingLocation START ===" << std::endl;
    std::cout << "Server pointer: " << server << std::endl;
    std::cout << "Path: [" << path << "]" << std::endl;
    
    (void)path;
    
    if (!server)
    {
        std::cout << "Server is NULL, returning NULL" << std::endl;
        return NULL;
    }
    
    std::cout << "Server is valid, getting locations..." << std::endl;
    
    try {
        const std::vector<Location*>& locations = server->getLocations();
        std::cout << "Got locations vector, size: " << locations.size() << std::endl;
        
        if (locations.empty()) {
            std::cout << "Locations vector is empty, returning NULL" << std::endl;
            return NULL;
        }
        
        std::cout << "Accessing locations[0]..." << std::endl;
        Location* result = locations[0];
        std::cout << "locations[0] pointer: " << result << std::endl;
        
        return result;
    }
    catch (const std::exception& e) {
        std::cout << "Exception in findMatchingLocation: " << e.what() << std::endl;
        return NULL;
    }
    catch (...) {
        std::cout << "Unknown exception in findMatchingLocation" << std::endl;
        return NULL;
    }
}
void    HttpServer::handleGetRequest(ClientData& client)
{
    std::cout << "=== handleGetRequest START ===" << std::endl;
    
    std::cout << "client.parseRequest pointer: " << client.parseRequest << std::endl;
    if (!client.parseRequest) {
        std::cout << "ERROR: client.parseRequest is NULL!" << std::endl;
        return;
    }
    HttpRequest* req = client.parseRequest;
    std::cout << "Got HttpRequest pointer: " << req << std::endl;
    
    std::cout << "About to call extractPath..." << std::endl;
    std::string path = extractPath(req->getUrl());
    std::cout << "Extracted path: [" << path << "]" << std::endl;

    if (path == "/")
        path = "/index.html";
    std::cout << "Final path: [" << path << "]" << std::endl;
    // std::string filePath = buildFilePath(client.location, path);
    // std::ifstream file(filePath.c_str());
    std::cout << "About to call buildFilePath..." << std::endl;
    std::cout << "client.location pointer: " << client.location << std::endl;
    
    // buildFilePath 대신 직접 경로 설정으로 테스트
    std::string filePath = "./www" + path;
    std::cout << "Using direct file path: [" << filePath << "]" << std::endl;
    
    std::cout << "About to open file..." << std::endl;
    std::ifstream file(filePath.c_str());

    if (file.good())
    {
        std::cout << "File opened successfully" << std::endl;
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        std::cout << "File read successfully, size: " << content.length() << std::endl;
        std::string contentType = "text/html";  // 기본값
        size_t dotPos = filePath.find_last_of('.');
        if (dotPos != std::string::npos) {
            std::string extension = filePath.substr(dotPos);
            if (extension == ".css")
                contentType = "text/css";
            else if (extension == ".js")
                contentType = "application/javascript";
            else if (extension == ".html" || extension == ".htm")
                contentType = "text/html";
            else if (extension == ".png")
                contentType = "image/png";
            else if (extension == ".jpg" || extension == ".jpeg")
                contentType = "image/jpeg";
        }
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: " << contentType << "\r\n";  // 동적 Content-Type
        response << "Content-Length: " << content.length() << "\r\n";
        response << "\r\n";
        response << content;

        client.response = response.str();
        std::cout << "Response prepared, size: " << client.response.length() << std::endl;
        std::cout << "Serving file: " << filePath << std::endl;
    }
    else
    {
        std::cout << "File not found: " << filePath << std::endl;
        std::string notFound = "<!DOCTYPE html><html><body><h1>404 Not Found</h1></body></html>";
        std::ostringstream  response;
        response << "HTTP/1.1 404 Not Found\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << notFound.length() << "\r\n";
        response << "\r\n";
        response << notFound;

        client.response = response.str();
        // std::cout << "File not found: " << filePath << std::endl;
    }
    std::cout << "=== handleGetRequest END ===" << std::endl;
}

void    HttpServer::handlePostRequest(ClientData& client)
{
    HttpRequest* req = client.parseRequest;
    std::string body = req->getBody();

    std::cout << "POST data received: " << body.length() << " bytes" << std::endl;

    std::string responseBody = "POST data received successfully";
    std::ostringstream  response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/plain\r\n";
    response << "Content-Length: " << responseBody.length() << "\r\n";
    response << "\r\n";
    response << responseBody;

    client.response = response.str();
}

void    HttpServer::handleDeleteRequest(ClientData& client)
{
    HttpRequest* req = client.parseRequest;
    std::string path = extractPath(req->getUrl());

    std::cout << "DELETE request for: " << path << std::endl;

    std::string responseBody = "DELETE request processed";
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/plain\r\n";
    response << "Content-Length: " << responseBody.length() << "\r\n";
    response << "\r\n";
    response << responseBody;

    client.response = response.str();
}

void    HttpServer::handleCgiRequest(ClientData& client, LocationCGI* cgiLocation)
{
    (void)cgiLocation;
    client.response = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 15\r\n\r\nCGI Not Ready";
}

std::string HttpServer::getMethodString(HttpMethod method)
{
    switch (method)
    {
        case METHOD_GET: return "GET";
        case METHOD_POST: return "POST";
        case METHOD_DELETE: return "DELETE";
        default: return "UNKNOWN";
    }
}

std::string HttpServer::extractPath(const std::string& url)
{
    size_t  queryPos = url.find('?');
    if (queryPos != std::string::npos)
        return url.substr(0, queryPos);
    return url;
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
    
    std::cout << "Client found, responseReady: " << it->second.responseReady << std::endl;  // 추가
    
    if (!it->second.responseReady)
    {
        std::cout << "Response not ready" << std::endl;
        return -1;
    }
    
    const std::string& response = it->second.response;
    std::cout << "Response size: " << response.length() << " bytes" << std::endl;  // 추가
    
    ssize_t sent = send(clientFd, response.c_str(), response.length(), 0);
    
    if (sent < 0)
    {
        perror("send failed");  // 구체적인 에러 확인
        return -1;
    }
    
    std::cout << "Response sent: " << sent << " bytes" << std::endl;
    return sent;
}

const char *HttpServer::FailedSocket::what() const throw()
{
    return ("Here's Johnny!");
}

HttpServer::~HttpServer()
{
}