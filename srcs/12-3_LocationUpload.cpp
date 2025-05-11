#include "webserv.hpp"

LocationUpload::LocationUpload()
{

}

LocationUpload::LocationUpload(const LocationUpload& rhs): Location(rhs)
{
    try
        this->_uploadStore = rhs.getUploadStore();
    catch (const std::exception& e)
        this->_uploadStore.clear();
    
}

LocationUpload& LocationUpload::operator=(const LocationUpload& rhs)
{
    if (this != &rhs)
    {
        Location::operator=(rhs);

        try
            this->_uploadStore = rhs.getUploadStore();
        catch (const std::exception& e)
            this->_uploadStore.clear();
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

std::string LocationUpload::getUploadStore() const
{
    return (this->_uploadStore);
}

bool    LocationUpload::hasUploadStore() const
{
    if (this->_uploadStore.empty())

}
