#include "20_HttpServer.hpp"

HttpServer::HttpServer(const Config& config) : _config(config), _maxFd(0)
{
    FD_ZERO(&_readFds);
    FD_ZERO(&_writeFd);
}

void    HttpServer::initialize()
{
    setupServerSokets();
}

int    HttpServer::setupServerSokets()
{
    const   std::vector<Server>& servers = _config.getServers();

    for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end())
    {
        const Server& server = *it;
        int port = server.getPort();

        if (_serverSockerts.find(port) != _serverSockerts.end())
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
    }
}