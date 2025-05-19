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
            perror("socket");
            return (-1);
        }
        int opt = 1;
        if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            perror("setsockopt");
            close(serverFd);
            return (-1);
        }

        int flags = fcntl(serverFd, F_GETFL, 0);
        if (flags == -1)
        {
            perror("fcntl(F_GETFL)");
            return (-1);
        }
        if (fcntl(serverFd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            perror("fcntl(F_SETFL)");
            return (-1);
        }
        struct sockaddr_in  socketAddr;
        std::memset(&socketAddr, 0, sizeof(socketAddr));
        socketAddr.sin_family = AF_INET;

    }
}