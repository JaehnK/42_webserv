#pragma once
#include "webserv.hpp"

class LocationCGI: public Location
{
    private:
        std::string                         _pass;
        std::string                         _cgiIndex;
        std::map<std::string, std::string>  _param;
        
        void    initParam();
    public:
    
        LocationCGI();
        LocationCGI(const LocationCGI& rhs);
        LocationCGI& operator=(const LocationCGI& rhs);
        ~LocationCGI();


        // setter
        void    setPass(std::string pass);
        void    setcgiIndex(std::string idx);
        void    addParam(std::string name, std::string value);

        // getter
        std::string                         getPass() const;
        std::string                         getCgiIndex() const;
        std::map<std::string, std::string>  getParam() const;;

        // checker
        bool    hasPass() const;
        bool    hasParam() const;

        virtual LocationCGI* clone() const;
};