#pragma once
#include "webserv.hpp"

class LocationUpload: public Location
{
    private:
        std::string _uploadStore;
        std::string _clientBodyTempPath;
        bool        _clientBodyinFileOnly;
    
    public:
        LocationUpload();
        LocationUpload(const LocationUpload& rhs);
        LocationUpload& operator=(const LocationUpload& rhs);
        ~LocationUpload();
        
        void        setUploadStore(std::string uploadStore);
        void        setClientBodyTempPath(std::string cbtp);
        void        setClientBodyFileOnly(bool cbfo);

        std::string getUploadStore() const;
        std::string setClientBodyTempPath() const;
        bool        getClientBodyFileOnly() const;

        bool        hasUploadStore() const;
};