#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <string>

class ServerSocket
{
    private:
        int _fd;
        int _port;
        struct sockaddr_in  _addr;

        void    configureSocket();
        void    bindAndListen();

    public:
        ServerSocket(int port);
        ~ServerSocket();

        int getFd() const;
        int getPort() const;

        class SocketCreationError: public std::exception
        {
            const char* what() const throw() {
                return "Failed to create server socket";
            }
        };

        class SocketBindError: public std::exception
        {
            const char* what() const throw() {
                return "Failed to bind on server socket";
            }
        };
};
