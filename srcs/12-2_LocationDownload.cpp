#include "webserv.hpp"

LocationDownload::LocationDownload()
{
    this->_locType = DOWNLOAD;
    this->_autoIndex = false;
    this->_addHeader = std::map<std::string, std::string> ();
}

LocationDownload::LocationDownload(const LocationDownload& rhs)
{
    *this = rhs;
}

LocationDownload& LocationDownload::operator=(const LocationDownload& rhs)
{
    if (this != &rhs)
    {
        Location::operator=(rhs);
        this->_autoIndex = rhs.getAutoIndex();
        this->_addHeader = rhs.getAddHeader();
    }
    return (*this);
}

LocationDownload::~LocationDownload()
{

}

void    LocationDownload::setAutoIndex(bool ai)
{
    this->_autoIndex = ai;
}

void    LocationDownload::addAddHeader(std::string name, std::string value)
{
    this->_addHeader[name] = value;
}

bool   LocationDownload::getAutoIndex() const
{
    return (this->_autoIndex);
}

std::map<std::string, std::string>    LocationDownload::getAddHeader() const
{
    return (this->_addHeader);
}

bool    LocationDownload::hasAddHeader() const
{
    if (this->_addHeader.size() == 0)
        return (false);
    return (true);
}

LocationDownload*   LocationDownload::clone() const
{
    return new LocationDownload(*this);
}
