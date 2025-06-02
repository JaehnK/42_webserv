#pragma once
#include "webserv.hpp"

class HttpResponse
{
    private:
        std::string                        _protocol;
        int                                _statusCode;
        std::string                        _statusMsg;
        std::map<std::string, std::string> _headers;
        std::string                        _body;
        std::string                        _responseMsg;
        HttpRequest&                       _request;

        HttpResponse();
    public:
        HttpResponse(const HttpRequest& request);
        ~HttpResponse();

        void    setStatusCode(int code);
        void    setHeader(const std::string &key, const std::string& value);
        void    setBody(const std::string& body);

        std::string build() const;
        static std::string getStatusMessage(int code);
};
