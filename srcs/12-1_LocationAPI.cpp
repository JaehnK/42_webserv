#include "webserv.hpp"

LocationAPI::LocationAPI()
    : Location()
{
    this->_locType = API;
    this->_return = std::map<int, std::string> ();
}

LocationAPI::LocationAPI(const LocationAPI& rhs): Location(rhs)
{
    std::map<int, std::string> tmpReturn = rhs.getReturn();

    this->_locType = API;
    this->_return = tmpReturn;
}

LocationAPI&    LocationAPI::operator=(const LocationAPI& rhs)
{
    if (this != &rhs)
    {
        std::map<int, std::string> tmpReturn = rhs.getReturn();
        Location::operator=(rhs);
        this->_return = tmpReturn;
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
    return (this->_return);
}

bool    LocationAPI::hasReturn() const
{
    if (this->_return.size() == 0)
        return (false);
    return (true);
}

LocationAPI*    LocationAPI::clone() const
{
    return (new LocationAPI(*this));
}

