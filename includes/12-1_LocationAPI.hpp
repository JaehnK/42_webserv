#pragma once
#include "webserv.hpp"

class LocationAPI: public Location
{
    private:
        std::map<int, std::string> _return;

    public:
        LocationAPI();
        LocationAPI(const LocationAPI& rhs);
        LocationAPI&    operator=(const LocationAPI& rhs);
        ~LocationAPI();

        void                        addReturn(int key, std::string val);
        std::map<int, std::string>  getReturn() const;
        bool                        hasReturn() const;

};