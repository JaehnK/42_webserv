#pragma once
#include "IResponse.hpp"
#include "21_HttpRequest.hpp"

class DeleteResponse : public IResponse
{
    private:
        HttpRequest _req;
        std::string _resp;

    public:
        DeleteResponse(const HttpRequest& req);
        void        handleRequest();
        std::string getTotalResp() const;
};