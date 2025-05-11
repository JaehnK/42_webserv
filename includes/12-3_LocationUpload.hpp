#pragma once
#include "webserv.hpp"

class LocationUpload: public Location
{
    private:
        std::string _uploadStore;
    
    public:
        LocationUpload();
        LocationUpload(const LocationUpload& rhs);
        LocationUpload& operator=(const LocationUpload& rhs);
        ~LocationUpload();
        
        void        setUploadStore(std::string uploadStore);
        std::string getUploadStore() const;
        bool        hasUploadStore() const;
}