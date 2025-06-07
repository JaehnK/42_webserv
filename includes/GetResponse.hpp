#pragma once
#include "IResponse.hpp"
#include "21_HttpRequest.hpp"

class GetResponse : public IResponse
{
    private:
        HttpRequest _req;
        std::string _body, _response, _mimeType;
        int         _statusCode;

        void        assembleResponse();
        std::string getMimeType(const std::string& ext) const;
        std::string getSatusText(int code) const;

    public:
        GetResponse(const HttpRequest& req);
        GetResponse(const GetResponse& obj);
        GetResponse& operator=(const GetResponse &obj);
        ~GetResponse();
        
        void        handleRequest();
        std::string getTotalResp() const;
};