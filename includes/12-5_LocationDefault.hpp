#pragma once
#include "webserv.hpp"

class LocationDefault: public Location
{
    private:
        /* data */
    public:
        LocationDefault();
        LocationDefault(const LocationDefault& rhs);
        LocationDefault&    operator=(const LocationDefault& rhs);
        ~LocationDefault();
};