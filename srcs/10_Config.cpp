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
        addValidateMsg("Global: client_max_body_size not defined, using default: 1MB");
        this->_clientMaxBodySize = 1048576; // 1mb 값
    }

    if (this->_servers.empty())
    {
        addValidateMsg("Global: No Servers Configured");
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
            addValidateMsg("Server: Duplicated Listens");
            isVal = false;
        }
        seenListens.insert(listen);

        if (!validateServer(this->_servers[i], i))
            isVal = false;
    }

}

bool    Config::validateServer(ServerConfig& server, int idx)
{
    bool    isVal = true;
    std::ostringstream  ss;
    std::string         context;
    
    ss << "Server[" << idx << "]: ";
    context = ss.str();

    if (!server.hasHost() || server.getHost().empty())
    {
        server.setHost("0.0.0.0");
        addValidateMsg(context + "host not defined, using default 0.0.0.0");
    }
    
    if (!server.hasPort() || server.getPort() <= 0)
    {
        server.setPort(80);
        addValidateMsg(context + "port not defined, using default 0.0.0.0");
    }
    else if (server.getPort() > 65535)
    {
        addValidateMsg(context + "invalid port number");
    }

    if (!server.hasName() || server.getName().empty())
    {
        server.setName("_");
        addValidateMsg(context + "server_name not defind, using default _");
    }
    
    if (!server.hasRoot() || server.getRoot().empty())
    {
        server.setRoot("/var/www");
        addValidateMsg(context + "root not defined, using default var/www");
    }
    else
    {
        struct stat st;
        if (stat(server.getRoot().c_str(), &st) != 0)
        {
            addValidateMsg(context + "invalid root directory");
            isVal = false;
        }
    }

    if (!validateLocations(server, idx))
        isVal = false;
    
    return (isVal);
}

bool    Config::validateLocations(ServerConfig& server, int idx)
{
    bool    isVal = true;
    (void) server;
    idx++; // 나중에 와서 만들거임
    return (isVal);
}