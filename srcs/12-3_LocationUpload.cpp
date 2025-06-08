#include "webserv.hpp"

LocationUpload::LocationUpload()
{
    this->_locType = UPLOAD;

}

LocationUpload::LocationUpload(const LocationUpload& rhs): Location(rhs)
{
    *this = rhs;    
}

LocationUpload& LocationUpload::operator=(const LocationUpload& rhs)
{
    if (this != &rhs)
    {
        Location::operator=(rhs);

        try
        {
            this->_uploadStore = rhs.getUploadStore();
        }
        catch (const std::exception& e)
        {
            this->_uploadStore.clear();
        }
    }

    return (*this);
}

LocationUpload::~LocationUpload()
{

}


void    LocationUpload::setUploadStore(std::string uploadStore)
{
    this->_uploadStore = uploadStore;
}

void    LocationUpload::setClientBodyTempPath(std::string cbtp)
{
    this->_clientBodyTempPath = cbtp;
}

void    LocationUpload::setClientBodyFileOnly(bool cbfo)
{
    this->_clientBodyinFileOnly = cbfo;
}

std::string LocationUpload::getUploadStore() const
{
    return (this->_uploadStore);
}

std::string LocationUpload::setClientBodyTempPath() const
{
    return (this->_clientBodyTempPath);
}

bool        LocationUpload::getClientBodyFileOnly() const
{
    return (this->_clientBodyinFileOnly);
}



bool    LocationUpload::hasUploadStore() const
{
    if (this->_uploadStore.empty())
        return (false);
    return (true);

}

LocationUpload* LocationUpload::clone() const
{
    return (new LocationUpload(*this));
}