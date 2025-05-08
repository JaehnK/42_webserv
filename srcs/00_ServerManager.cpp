#include "webserv.hpp"

ServerManager::ServerManager()
{

}

ServerManager::ServerManager(const ServerManager &rhs)
{
    *this = rhs;
}

ServerManager &ServerManager::operator=(const ServerManager &rhs)
{
    if (this != &rhs)
    {

    }
    return (*this);
}

ServerManager::~ServerManager()
{

}

int    ServerManager::setConfig(char *fileName)
{
    this->conf = Config(fileName);
    return (0);
}

int    ServerManager::setConfig()
{
    return (0);
}
