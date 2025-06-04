#include "20_HttpServer.hpp"

HttpServer::HttpServer(const Config& config) : _config(config)
{
}

void    HttpServer::initialize()
{
    setupServerSockets();
    initializeEpoll();
}

int     HttpServer::createServerSocket(int port)
{
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw SocketCreationError();
	try {
		configureSocket(fd);
		bindAndListen(fd, port);
		return fd;
	} catch (const std::exception& e) {
		close(fd);
		throw;
	}
}

void	HttpServer::configureSocket(int fd)
{
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw SocketConfigError();

	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw SocketConfigError();
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw SocketConfigError();
}

void	HttpServer::bindAndListen(int fd, int port)
{
	struct sockaddr_in socketAddr;
	std::memset(&socketAddr, 0, sizeof(socketAddr));
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	socketAddr.sin_port = htons(port);

	if (bind(fd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) < 0)
		throw SocketConfigError();

	if (listen(fd, 10) < 0)
		throw SocketConfigError();
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

		try {
			int fd = createServerSocket(port);
			_serverSockets[port] = fd;
		} catch (const std::exception& e) {
            std::cerr << "Failed to setup server socket for port " << port << ": " << e.what() << std::endl;
            throw;
		}
        std::cout << "Server listening on port " << port << std::endl;
    }
    return (0);
}

void    HttpServer::initializeEpoll()
{
    for (std::map<int, int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); ++it)
		_epoll.add(it->second, EPOLLIN);
}

bool    HttpServer::isServerSocket(int fd)
{
	for (std::map<int, int>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); ++it)
		if (it->second == fd)
			return true;
	return false;
}

void	HttpServer::handleEvent(const epoll_event& event)
{
	int fd = event.data.fd;

	if (isServerSocket(fd))
		acceptNewConnection(fd);
	else
	{
		if (event.events & EPOLLERR || event.events & EPOLLHUP)
		{
			std::cerr << "Error or hangup on socket (fd: " << fd << ")" << std::endl;
			closeClientConnection(fd);
			return;
		}
		if (event.events & EPOLLIN)
		{
			handleClientRead(fd);
		}
		else if (event.events & EPOLLOUT)
		{
			handleClientWrite(fd);
		}
	}
}

void    HttpServer::run()
{
    int	eventCount;

    while (1)
    {
        std::cout << "Waiting for events..." << std::endl;
		try {
			eventCount = _epoll.wait(_events);
		} catch (const std::exception& e) {
			std::cerr << "Epoll error: " << e.what() << std::endl;
			break;
		}
        for (int i = 0; i < eventCount; i++)
			handleEvent(_events[i]);
    }
}

void HttpServer::acceptNewConnection(int serverFd) 
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
    
	try {
		_epoll.add(clientFd, EPOLLIN);
	} catch (std::exception& e) {
		std::cerr << "Failed to add client to epoll" << std::endl;
		_clients.erase(clientFd);
		close(clientFd);
		return;
	}
    std::cout << "Client successfully registered" << std::endl;
}

int     HttpServer::getServerPort(int serverFd)
{
    int port;

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

bool    HttpServer::setupClientSocket(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << "Failed to get socket flags" << std::endl;
        return (false);
    }
    
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
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

int HttpServer::handleClientRead(int currentFd)
{
    std::cout << "=== handleClientRead for fd: " << currentFd << " ===" << std::endl;
    
    // 클라이언트가 맵에 있는지 먼저 확인
    std::map<int, ClientData>::iterator client_it = _clients.find(currentFd);
    if (client_it == _clients.end())
    {
        std::cerr << "Client not found in map for fd: " << currentFd << std::endl;
        closeClientConnection(currentFd);
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
			_epoll.modify(currentFd, EPOLLOUT);
        }
        else
        {
            std::cout << "Response not ready!" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request processing error: " << e.what() << std::endl;
        closeClientConnection(currentFd);
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
            closeClientConnection(currentFd);
        }
        return (1);
    }

    closeClientConnection(currentFd);
    return (0);
}

void    HttpServer::closeClientConnection(int fd)
{
    std::map<int, ClientData>::iterator it = _clients.find(fd);

    if (it != _clients.end()) {
        std::cout << "Closing client connection (fd: " << fd << ")" << std::endl;
		_epoll.remove(fd);
        close(fd);
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

ClientData& HttpServer::getClientData(int fd)
{
    std::cout << "Looking for client fd: " << fd << " in map of size: " << _clients.size() << std::endl;
    
    std::map<int, ClientData>::iterator client_it = _clients.find(fd);
    
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

const char *HttpServer::SocketCreationError::what(void) const throw()
{
	return ("Socket creation failed: ");
}

const char *HttpServer::SocketConfigError::what() const throw()
{
	return ("Socket configuration failed: ");
}