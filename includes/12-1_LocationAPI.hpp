#pragma once
#include "webserv.hpp"

class LocationAPI: public Location
{
    private:
        std::vector<std::map<int, std::string> > _return;

    public:
        LocationAPI();
        LocationAPI(const LocationAPI& rhs);
        LocationAPI&    operator=(const LocationAPI& rhs);
        ~LocationAPI();

        void                                        addReturn(std::map<int, std::string> ret);
        std::vector<std::map<int, std::string> >    getReturn() const;
        bool                                        hasReturn() const;

};