#include "webserv.hpp"

ServerConfig::ServerConfig()
{
    this->_port = -1;
    this->_errorPages = std::map<int, std::string> ();
    this->_locations = std::vector<Location*> ();
}

ServerConfig::ServerConfig(const ServerConfig &rhs)
{
    *this = rhs;
}

ServerConfig& ServerConfig::operator=(const ServerConfig &rhs)
{
    if (this != &rhs)
    {
        this->_errorPages = rhs.getErrorPages();
        this->_locations = rhs.getLocations();
        this->_host = rhs.getHost();
        this->_listen = rhs.getListen();
        this->_port = rhs.getPort();
        this->_root = rhs.getRoot();
        this->_name = rhs.getName();

    }
    return (*this);
}

ServerConfig::~ServerConfig()
{
    // for (std::vector<Location*>::iterator it = _locations.begin(); 
    //          it != _locations.end(); ++it) 
    // {
    //     delete *it;
    // }
}

void    ServerConfig::setName(const std::string& name)
{
    this->_name = name;
}

void    ServerConfig::setHost(const std::string& host)
{
    this->_host = host;
}

void    ServerConfig::setPort(int port)
{
    this->_port = port;
}

void    ServerConfig::setListen(const std::string& listen)
{
    this->_listen = listen;
}

void    ServerConfig::setRoot(const std::string& root)
{
    this->_root = root;
}

void    ServerConfig::addErrorPage(int key, std::string value)
{
    this->_errorPages[key] = value;
}

void    ServerConfig::addLocation(Location* location)
{
    this->_locations.push_back(location);
}

std::string ServerConfig::getName() const
{
    // if (this->hasName() == false)
    //     throw DataNotFoundException();
    return (this->_name);
}

std::string ServerConfig::getHost() const
{
    // if (this->hasHost() == false)
    //     throw DataNotFoundException();
    return (this->_host);
}

int ServerConfig::getPort() const
{
    // if (this->hasPort() == false)
    //     throw DataNotFoundException();
    return (this->_port);
}

std::string ServerConfig::getListen() const
{
    // if (this->hasListen() == false)
    //     throw DataNotFoundException();
    return (this->_listen);
}

std::string ServerConfig::getRoot() const
{
    // if (this->hasRoot() == false)
    //     throw DataNotFoundException();
    return (this->_root);
}

std::map<int, std::string>    ServerConfig::getErrorPages() const
{
    // if (this->hasErrorPages() == false)
    //     throw DataNotFoundException(); 
    return (this->_errorPages);
}

std::vector<Location*>   ServerConfig::getLocations() const
{
    // if (this->hasLocations() == false)
    //     throw DataNotFoundException();
    return (this->_locations);
}

bool    ServerConfig::hasName() const
{
    if (this->_name.empty())
        return (false);
    return (true);
}

bool    ServerConfig::hasHost() const
{
    if (this->_host.empty())
        return (false);
    return (true);
}

bool    ServerConfig::hasPort() const
{
    if (this->_port == -1)
        return (false);
    return (true);
}

bool    ServerConfig::hasListen() const
{
    if (this->_listen.empty())
        return (false);
    return (true);
}

bool    ServerConfig::hasRoot() const
{
    if (this->_root.empty())
        return (false);
    return (true);
}

bool    ServerConfig::hasErrorPages() const
{
    if (this->_errorPages.size() == 0)
        return (false);
    return (true);
}

bool    ServerConfig::hasLocations() const
{
    if (this->_locations.size() == 0)
        return (false);
    return (true);
}

size_t  ServerConfig::errPagesSize() const
{
    return (this->_errorPages.size());
}

size_t  ServerConfig::locationSize() const
{
    return (this->_locations.size());
}

const char* ServerConfig::DataNotFoundException::what() const throw()
{
    return ("Data does not exist.");
}

std::ostream	&operator<<(std::ostream& os, const ServerConfig& serv)
{
    os << "Server Settings: " << std::endl;
    os << "  name: " << serv.getName() << "\n";
    os << "  host: " << serv.getHost() << "\n";
    os << "  port: " << serv.getPort() << "\n";
    os << "  listen: " << serv.getListen() << "\n";
    os << "  errorPages Size: " << serv.errPagesSize() << "\n";
    os << "  location Size: " << serv.locationSize() << std::endl;
    return (os);
}