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