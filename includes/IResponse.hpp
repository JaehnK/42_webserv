#pragma once
#include <string>

class IResponse
{
    public:
        virtual ~IResponse() {}
        virtual void handleRequest() = 0;
        virtual std::string getTotalResp() const = 0;
};