#pragma once
#include "webserv.hpp"

class Location
{
    protected:
        std::string                             _path;
        int                                     _clientMaxBodySize;
        std::string                             _root;
        std::vector<std::string>                _index;
        std::vector<std::string>                _limitExcept;
        std::vector<std::map<int, std::string>  _errorPage;
        bool                                    _denyAll;
    
    public:
        Location();
        Location(const Location& rhs);
        Location&   operator=(const Location& rhs);
        ~Location();

        // Setters
        void    setPath(std::string path);
        void    setClientMaxBodySize(int cmbs);
        void    setRoot(std::string root);

};


class LocationDownload: public Location
{
    private:
        bool    autoIndex;
        std::vector<std::string> addHeader;
}


class LocationUpload: public Location
{
    private:
        std::string uploadStore;

}

class LocationCGI: public Location
{
    private:
        std::string                 _pass;
        std::vector<std::string>    _param;
        bool                        includeParam;
        bool                        includePass;
}
