#pragma once
#include "webserv.hpp"

class Config;

class ServerManager
{
    private:
        Config  conf;
        
    public:
        ServerManager();
        ServerManager(const ServerManager &rhs);
        ServerManager &operator =(const ServerManager &rhs);
        ~ServerManager();

        int     setConfig(char *fileName);
        int     setConfig();

        // utils
        static std::vector<std::string>    split(const std::string &s, char delimiter);
};

