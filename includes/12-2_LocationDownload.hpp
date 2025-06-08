#pragma once
#include "webserv.hpp"

class LocationDownload: public Location
{
    private:
        bool                                _autoIndex;
        std::map<std::string, std::string>  _addHeader;
    
    public:
        LocationDownload();
        LocationDownload(const LocationDownload& rhs);
        LocationDownload& operator=(const LocationDownload& rhs);
        ~LocationDownload();

        // setter
        void    setAutoIndex(bool ai);
        void    addAddHeader(std::string name, std::string value);

        // getter
        bool                                getAutoIndex() const;
        std::map<std::string, std::string>  getAddHeader() const;

        // checker
        bool    hasAddHeader() const;

        virtual LocationDownload*           clone() const;
};