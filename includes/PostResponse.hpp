#pragma once
#include "IResponse.hpp"
#include "21_HttpRequest.hpp"

class PostResponse : public IResponse
{
    private:
        HttpRequest _req;
        std::string _resp;

    public:
        PostResponse(const HttpRequest& req);
        void        handleRequest();
        std::string getTotalResp() const;
};