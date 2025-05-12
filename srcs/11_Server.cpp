#include "webserv.hpp"

Server::Server()
{
    this->_port = -1;
    this->_errorPages = std::vector<std::map<int, std::string> > ();
    this->_locations = std::vector<Location> ();
}

Server::Server(const Server &rhs)
{
    *this = rhs;
}

Server& Server::operator=(const Server &rhs)
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

Server::~Server()
{
}

void    Server::setName(const std::string& name)
{
    this->_name = name;
}

void    Server::setHost(const std::string& host)
{
    this->_host = host;
}

void    Server::setPort(int port)
{
    this->_port = port;
}

void    Server::setListen(const std::string& listen)
{
    this->_listen = listen;
}

void    Server::setRoot(const std::string& root)
{
    this->_root = root;
}

void    Server::addErrorPage(const std::map<int, std::string>& errPage)
{
    this->_errorPages.push_back(errPage);
}

void    Server::addLocation(const Location& location)
{
    this->_locations.push_back(location);
}

std::string Server::getName() const
{
    if (this->hasName() == false)
        throw DataNotFoundException();
    return (this->_name);
}

std::string Server::getHost() const
{
    if (this->hasHost() == false)
        throw DataNotFoundException();
    return (this->_host);
}

int Server::getPort() const
{
    if (this->hasPort() == false)
        throw DataNotFoundException();
    return (this->_port);
}

std::string Server::getListen() const
{
    if (this->hasListen() == false)
        throw DataNotFoundException();
    return (this->_listen);
}

std::string Server::getRoot() const
{
    if (this->hasRoot() == false)
        throw DataNotFoundException();
    return (this->_root);
}

std::vector<std::map<int, std::string> >    Server::getErrorPages() const
{
    if (this->hasErrorPages() == false)
        throw DataNotFoundException(); 
    return (this->_errorPages);
}

std::vector<Location>   Server::getLocations() const
{
    if (this->hasLocations() == false)
        throw DataNotFoundException();
    return (this->_locations);
}

bool    Server::hasName() const
{
    if (this->_name.empty())
        return (false);
    return (true);
}

bool    Server::hasHost() const
{
    if (this->_host.empty())
        return (false);
    return (true);
}

bool    Server::hasPort() const
{
    if (this->_port == -1)
        return (false);
    return (true);
}

bool    Server::hasListen() const
{
    if (this->_listen.empty())
        return (false);
    return (true);
}

bool    Server::hasRoot() const
{
    if (this->_root.empty())
        return (false);
    return (true);
}

bool    Server::hasErrorPages() const
{
    if (this->_errorPages.size() == 0)
        return (false);
    return (true);
}

bool    Server::hasLocations() const
{
    if (this->_locations.size() == 0)
        return (false);
    return (true);
}

const char* Server::DataNotFoundException::what() const throw()
{
    return ("Data does not exist.");
}
