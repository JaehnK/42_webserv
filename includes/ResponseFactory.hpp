#pragma once

#include "IResponse.hpp"
#include "GetResponse.hpp"
#include "21_HttpRequest.hpp"
#include <stdexcept>

class ResponseFactory
{
    public:
        static IResponse* create(const HttpRequest& req);
};