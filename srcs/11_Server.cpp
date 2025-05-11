#include "webserv.hpp"

Server::Server()
{
    this->_port = -1;
    this->_errorPage = std::vector<std::map<int, std::string> > ();
    this->_location = std::vector<Location> ();
}

Server::Server(const Server &rhs)
{
    *this = rhs;
}

Server& Server::operator=(const Server &rhs)
{

}

Server::~Server()
{
}

