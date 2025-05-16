#include "webserv.hpp"

LocationAPI::LocationAPI()
{
    this->_locType = API;
    this->_return = std::map<int, std::string> ();
}

LocationAPI::LocationAPI(const LocationAPI& rhs): Location(rhs)
{
    *this = rhs;
}

LocationAPI&    LocationAPI::operator=(const LocationAPI& rhs)
{
    if (this != &rhs)
    {
        Location::operator=(rhs);
        this->_return = rhs.getReturn();
    }
    return (*this);
}

LocationAPI::~LocationAPI()
{

}

void    LocationAPI::addReturn(int key, std::string val)
{
    this->_return[key] = val;
}

std::map<int, std::string>    LocationAPI::getReturn() const
{
    if (this->hasReturn() == false)
        throw DataNotFoundException();
    return (this->_return);
}

bool    LocationAPI::hasReturn() const
{
    if (this->_return.size() == 0)
        return (false);
    return (true);
}


