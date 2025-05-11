#pragma once
#include "webserv.hpp"

class LocationCGI: public Location
{
    private:
        std::string                 _pass;
        std::vector<std::string>    _param;
        bool                        _includeParam;
        bool                        _includePass;
    
    public:
        LocationCGI();
        LocationCGI(const LocationCGI& rhs);
        LocationCGI& operator=(const LocationCGI& rhs);
        ~LocationCGI();

        // setter
        void    setPass(std::string pass);
        void    setIncludeParam(bool param);
        void    setIncludePass(bool pass);
        void    addParam(std::string param);

        // getter
        std::string                 getPass() const;
        std::vector<std::string>    getParam() const;
        bool                        getIncludePass() const;
        bool                        getIncludeParam() const;

        // checker
        bool    hasPass() const;
        bool    hasParam() const;
}