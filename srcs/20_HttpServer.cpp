#include "20_HttpServer.hpp"

HttpServer::HttpServer(const Config& config) : _config(config), _maxFd(0)
{
    FD_ZERO(&_readFds);
    FD_ZERO(&_writeFd);
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
        FD_SET(serverFd, &_readFds);

        if (serverFd > _maxFd)
            _maxFd = serverFd;
        
        std::cout << "Server listening on port " << port << "std::endl";
    }
}

const char *HttpServer::FailedSocket::what() const throw()
{
    return ("Here's Johnny!");
}