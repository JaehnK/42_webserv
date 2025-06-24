#include "webserv.hpp"

Config::Config()
{
    this->_fileName = "default";
    this->_clientMaxBodySize = 0;
    this->_servers = std::vector<ServerConfig> ();
    this->_validated = true;
    this->_validateErrMsgs = std::vector<std::string> ();
}

Config::Config(char *fileName)
{
    this->_fileName = fileName;
    this->_clientMaxBodySize = 0;
    this->_servers = std::vector<ServerConfig> ();
    this->_validated = true;
    this->_validateErrMsgs = std::vector<std::string> ();
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

void    Config::addServer(ServerConfig server)
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

const std::vector<ServerConfig> &Config::getServers() const
{
    return (this->_servers);
}

void    Config::addValidateMsg(std::string msg)
{
    this->_validateErrMsgs.push_back(msg);
}

bool    Config::validate()
{
    if (this->_clientMaxBodySize < 1)
    {
        std::cout << "Global: client_max_body_size not defined, using default: 1MB" << std::endl;
        this->_clientMaxBodySize = 1048576; // 1mb 값
    }

    if (this->_servers.empty())
    {
        std::cout << "Global: No Servers Configured" << std::endl;
        return (false);
    }
    
}

bool    Config::validateServers()
{
    bool        isVal;
    std::string listen;
    std::set<std::string> seenListens;


    isVal = true;
    for (int i = 0; i < this->_servers.size(); i++)
    {
        listen = this->_servers[i].getListen();
        if (seenListens.find(listen) != seenListens.end())
        {
            std::cout << "Server: Duplicated Listens" << std::endl;
            isVal = false;
        }
        seenListens.insert(listen);

        if (!this->_servers[i].validateServer(i))
            isVal = false;
    }
    return (isVal);
}

bool    Config::validateLocations(ServerConfig& server, int idx)
{
    bool    isVal = true;
    (void) server;
    idx++; // 나중에 와서 만들거임
    return (isVal);
}