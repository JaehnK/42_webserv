#pragma once
#include "webserv.hpp"

class ServerConfig;

class Config
{
    private:
        std::string             _fileName;
        int                     _clientMaxBodySize;
        std::vector<ServerConfig>     _servers;
        bool                        _validated;
        std::vector<std::string>    _validateErrMsgs;

    public:
        Config();
        Config(char *fileName);
        Config(const Config &rhs);
        Config& operator=(const Config &rhs);
        ~Config();

        // Setters
        void    setFileName(std::string fileName);
        void    setClientMaxBodySize(int clientMaxBodySize);
        void    addServer(ServerConfig server);
        void    addValidateMsg(std::string msg);

        // Getters
        std::string         getFileName() const;
        int                 getClientMaxBodySize() const;
        const std::vector<ServerConfig> &getServers() const;

        // validations
        bool    validate();
        bool    validateServers();
        bool    validateServer(ServerConfig& server, int idx);
        bool    validateLocations(ServerConfig& server, int idx);
        bool    validateLocation(Location& location);
};

std::ostream	&operator<<(std::ostream& os, const Config& conf);