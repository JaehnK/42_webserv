#pragma once
#include "webserv.hpp"

class Config
{
    private:
        std::string             _fileName;
        int                     _clientMaxBodySize;
        std::vector<Server>     _server;

        void                    parseFile();

    public:
        Config();
        Config(char *fileName);
        Config(const Config &rhs);
        Config& operator=(const Config &rhs);

        void    setClientMaxBodySize();
        int     getClientMaxBodySize();

        ~Config();

        std::string getName() const;
};