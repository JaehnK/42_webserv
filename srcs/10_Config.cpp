#include "webserv.hpp"

Config::Config()
{
    this->_fileName = "default";
    this->_clientMaxBodySize = 0;
    this->_servers = std::vector<Server> ();
}

Config::Config(char *fileName)
{
    this->_fileName = fileName;
    this->_clientMaxBodySize = 0;
    this->_servers = std::vector<Server> ();
}

Config::Config(const Config &rhs)
{
    *this = rhs;
}

Config& Config::operator=(const Config &rhs)
{
    if (this != &rhs)
    {
        this->_fileName = rhs.getFileName();
        this->_clientMaxBodySize = rhs.getClientMaxBodySize();
        this->_servers = rhs.getServers();
    }
    return (*this);
}

Config::~Config()
{}

void    Config::setFileName(std::string fileName)
{
    this->_fileName = fileName;
}

void    Config::setClientMaxBodySize(int clientMaxBodySize)
{
    this->_clientMaxBodySize = clientMaxBodySize;
}

void    Config::addServer(Server server)
{
    this->_servers.push_back(server);
}

std::string Config::getFileName() const
{
    return (this->_fileName);
}

int Config::getClientMaxBodySize() const
{
    return (this->_clientMaxBodySize);
}

std::vector<Server> Config::getServers() const
{
    return (this->_servers);
}
