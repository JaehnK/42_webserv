#pragma once
#include "webserv.hpp"

class HttpRequest
{
    private:
    std::string                         _method;
    std::string                         _path;
    std::string                         _protocol;
    std::map<std::string, std::string>  _headers;
    std::string                         _body;

public:
    HttpRequest();
    HttpRequest(const std::string& requestData);
    ~HttpRequest();

    bool    parse(const std::string& requestData);

    std::string getMethod()                         const;
    std::string getPath()                           const;
    std::string getProtocol()                       const;
    std::string getHeader()                         const;
    std::map<std::string, std::string>  getHeader() const;
    std::string getBody()                           const;

    bool    isComplete() const;
};
