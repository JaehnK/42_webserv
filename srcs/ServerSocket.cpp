#include "ServerSocket.hpp"
#include <fcntl.h>
#include <cstring>
#include <iostream>

ServerSocket::ServerSocket(int port) : _port(port)
{
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
        throw SocketCreationError();
    configureSocket();
    bindAndListen();
    std::cout << "ServerSocket bind on fd : " << _fd << std::endl;
}

ServerSocket::~ServerSocket()
{
    if (_fd >= 0)
        close(_fd);
    std::cout << "ServerSocket Destroyed" << std::endl;
}

void    ServerSocket::configureSocket()
{
	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw SocketCreationError();
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
        throw SocketCreationError();
	// int flags = fcntl(_fd, F_GETFL, 0);
	// if (flags == -1)
	// 	throw SocketConfigError();
	// if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	// 	throw SocketConfigError();
}

void    ServerSocket::bindAndListen()
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr.sin_port = htons(_port);

    if (bind(_fd, (struct sockaddr*)&_addr, sizeof(_addr)) == -1)
        throw SocketBindError();
    if (listen(_fd, SOMAXCONN) == -1)
        throw SocketBindError();
}

int ServerSocket::getFd() const
{
    return _fd;
}

int ServerSocket::getPort() const
{
    return _port;
}