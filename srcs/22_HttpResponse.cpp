#include "webserv.hpp"

HttpResponse::HttpResponse()
{}

HttpResponse::HttpResponse(const HttpRequest& request)
    :_request(request)
{
    this->_request.get
}

HttpResponse::~HttpResponse()
{
    
}