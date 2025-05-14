#pragma once
#include "webserv.hpp"

class Server;

class Config
{
    private:
        std::string             _fileName;
        int                     _clientMaxBodySize;
        std::vector<Server>     _servers;

    public:
        Config();
        Config(char *fileName);
        Config(const Config &rhs);
        Config& operator=(const Config &rhs);
        ~Config();

        // Setters
        void    setFileName(std::string fileName);
        void    setClientMaxBodySize(int clientMaxBodySize);
        void    addServer(Server server);

        // Getters
        std::string         getFileName() const;
        int                 getClientMaxBodySize() const;
        std::vector<Server> getServers() const;
};

std::ostream	&operator<<(std::ostream& os, const Config& conf);