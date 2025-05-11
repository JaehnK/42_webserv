#pragma once
#include "webserv.hpp"

class Config
{
    private:
        std::string             _fileName;
        int                     _clientMaxBodySize;
        std::vector<Server>     _servers;

        void                    parseFile();

    public:
        Config();
        Config(char *fileName);
        Config(const Config &rhs);
        Config& operator=(const Config &rhs);
        ~Config();

        void    setFileName(std::string fileName);
        void    setClientMaxBodySize(int clientMaxBodySize);
        void    addServer(Server server);
        
        std::string         getFileName() const;
        int                 getClientMaxBodySize() const;
        std::vector<Server> getServers() const;
};

std::ostream	&operator<<(std::ostream& os, const Config& conf);