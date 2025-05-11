#pragma once
#include "webserv.hpp"

class LocationDownload: public Location
{
    private:
        bool                        _autoIndex;
        std::vector<std::string>    _addHeader;
    
    public:
        LocationDownload();
        LocationDownload(const LocationDownload& rhs);
        LocationDownload& operator=(const LocationDownload& rhs);
        ~LocationDownload();

        // setter
        void    setAutoIndex(bool ai);
        void    addAddHeader(std::string addHeader);

        // getter
        bool                        getAutoIndex() const;
        std::vector<std::string>    getAddHeader() const;

        // checker
        bool    hasAddHeader() const;
}