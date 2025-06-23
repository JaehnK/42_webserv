#include "webserv.hpp"

LocationDefault::LocationDefault()
{
    this->_locType = DEFAULT;
}

LocationDefault::LocationDefault(const LocationDefault& rhs)
{
    *this = rhs;
}

LocationDefault&    LocationDefault::operator=(const LocationDefault& rhs)
{
    if (this != &rhs)
    {
        Location::operator=(rhs);
    }
    return (*this);
}

LocationDefault::~LocationDefault()
{
    
}


LocationDefault* LocationDefault::clone() const
{
    return new LocationDefault(*this);
}
