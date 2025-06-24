#include "webserv.hpp"

ServerConfig::ServerConfig()
{
    this->_port = -1;
    this->_errorPages = std::map<int, std::string> ();
    this->_locations = std::vector<Location*> ();
}

ServerConfig::ServerConfig(const ServerConfig &rhs)
{
    this->_port = rhs.getPort();
    this->_host = rhs.getHost();
    this->_listen = rhs.getListen();
    this->_root = rhs.getRoot();
    this->_name = rhs.getName();
    this->_errorPages = rhs.getErrorPages();
    
    const std::vector<Location*>& rhsLocations = rhs.getLocations();
    for (std::vector<Location*>::const_iterator it = rhsLocations.begin();
         it != rhsLocations.end(); ++it)
    {
        if (*it != NULL)
            _locations.push_back((*it)->clone());
    }
}

ServerConfig& ServerConfig::operator=(const ServerConfig &rhs)
{
    if (this != &rhs)
    {
        for (std::vector<Location*>::iterator it = _locations.begin(); \
             it != _locations.end(); ++it)
            delete *it;
        _locations.clear();
        
        // 새로운 Location들 깊은 복사
        const std::vector<Location*>& rhsLocations = rhs.getLocations();
        for (std::vector<Location*>::const_iterator it = rhsLocations.begin();
             it != rhsLocations.end(); ++it)
        {
            if (*it != NULL) {
                _locations.push_back((*it)->clone());
            }
        }
        
        // 나머지 멤버들 복사
        _errorPages = rhs._errorPages;
        _host = rhs._host;
        _listen = rhs._listen;
        _port = rhs._port;
        _root = rhs._root;
        _name = rhs._name;
    }
    return (*this);
}

ServerConfig::~ServerConfig()
{
    for (std::vector<Location*>::iterator it = _locations.begin(); \
        it != _locations.end(); ++it)
    {
        delete *it;
        *it = NULL;
    }
    this->_locations.clear();
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

const Location* ServerConfig::matchLocation(const std::string& path)
{
    std::vector<Location*>::iterator it = this->_locations.begin();

    while (it != this->_locations.end())
    {
        if ((*it)->getPath() == path)
            return (*it);
        it++;
    }
    return (NULL);
}

bool    ServerConfig::validateServer(int idx)
{
    bool    isVal = true;
    std::ostringstream  ss;
    std::string         context;
    
    ss << "Server[" << idx << "]: ";
    context = ss.str();

    if (!this->hasHost() || this->getHost().empty())
    {
        this->setHost("0.0.0.0");
        std::cout << context + "host not defined, using default 0.0.0.0" << std::endl;
    }
    
    if (!this->hasPort() || this->getPort() <= 0)
    {
        this->setPort(80);
        std::cout << context + "port not defined, using default 80" << std::endl;
    }
    else if (this->getPort() > 65535)
    {
        std::cout << context + "invalid port number" << std::endl;
    }

    if (!this->hasName() || this->getName().empty())
    {
        this->setName("_");
        std::cout << context + "server_name not defind, using default _" << std::endl;
    }
    
    if (!this->hasRoot() || this->getRoot().empty())
    {
        this->setRoot("/var/www");
        std::cout << context + "root not defined, using default var/www" << std::endl;
    }
    else
    {
        struct stat st;
        if (stat(this->getRoot().c_str(), &st) != 0)
        {
            std::cout << context + "invalid root directory" << std::endl;
            isVal = false;
        }
    }

    if (!validateLocations(idx))
        isVal = false;
    
    return (isVal);

}

bool    ServerConfig::validateLocations(int idx)
{
    bool                     isVal = true;
    std::set<std::string>    seenPath;
    
    if (this->locationSize() == 0)
    {
        std::cerr << "Server[ " << idx << " ]: No locations defined" << std::endl; 
        isVal = false;
        return (isVal);
    }
    
    std::vector<Location*>::iterator it = this->_locations.begin();
    while (it != this->_locations.end())
    {
        if (seenPath.find((*it)->getPath()) != seenPath.end())
        {
            std::cerr << "Server[ " << idx << " ]:Duplicated Location Path" << std::endl;
            isVal = false;
        }

        if (!(*it)->validateLocation())
            isVal = false;
        seenPath.insert((*it)->getPath());
        it++;
    }
    return (isVal);
}



std::ostream	&operator<<(std::ostream& os, const ServerConfig& serv)
{
    os << "Server Settings: " << "\n";
    os << "  name: " << serv.getName() << "\n";
    os << "  host: " << serv.getHost() << "\n";
    os << "  port: " << serv.getPort() << "\n";
    os << "  listen: " << serv.getListen() << "\n";
    os << "  errorPages Size: " << serv.errPagesSize() << "\n";
    os << "  location Size: " << serv.locationSize() << std::endl;
    return (os);
}