#pragma once
#include "webserv.hpp"

class Server;

class Config
{
    private:
        std::string             _fileName;
        int                     _clientMaxBodySize;
        std::vector<Server>     _servers;

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
        void    addServer(Server server);
        void    addValidateMsg(std::string msg);

        // Getters
        std::string         getFileName() const;
        int                 getClientMaxBodySize() const;
        const std::vector<Server> &getServers() const;

        // validations
        bool    validate();
        bool    validateServers();
        bool    validateServer(Server& server, int idx);
        bool    validateLocations(Server& server, int idx);
        bool    validateLocation(Location& location);
};

std::ostream	&operator<<(std::ostream& os, const Config& conf);