#include "webserv.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::HttpRequest(int fd)
{
    this->_fd = fd;
    this->_method = IN_REQUEST;
    this->_headers = std::map<std::string, std::string> ();
}

HttpRequest::HttpRequest(const HttpRequest& rhs)
{
    *this = rhs;
}

HttpRequest&    HttpRequest::operator=(const HttpRequest& rhs)
{
    if (this != &rhs)
    {
        this->_fd = rhs.getFd();
        this->_method = rhs.getMethod();
        this->_body = rhs.getBody();
        this->_headers = rhs.getheaders();
        this->_contentLength = rhs.getContentLength();
        this->_bodyBytesRead = rhs.getBodyBytesRead();
        rhis->_state = rhs.getState();
    }
    return (*this);
}

HttpRequest::~HttpRequest()
{}

void	HttpRequest::setMethod(const std::string& method)
{
    this->_method = method;
}

void	HttpRequest::setBody(const std::string& body)
{
    this->_body = body;
}

void	HttpRequest::setContentLength(size_t length)
{
    this->_contentLength = length;
}

void	HttpRequest::setBodyBytesRead(size_t readBytes)
{
    this->_bodyBytesRead = readBytes;
}

void	HttpRequest::addHeader(const std::string& key, const std::string& value)
{
    this->_headers[key] = value;
}

int HttpRequest::getFd() const
{
    return (this->_fd);
}

HttpMethod	HttpRequest::getMethod() const
{
    return (this->_method);
}

std::string	HttpRequest::getBody() const
{
    return (this->_body);
}

std::map<std::string, std::string>	HttpRequest::getHeaders() const
{
    return (this->_headers);
}

size_t	    HttpRequest::getContentLength() const
{
    return (this->_contentLength);
}

size_t	    HttpRequest::getBodyBytesRead() const
{
    return (this->_bodyBytesRead);
}

ReqState    HttpRequest::getState() const
{
    return (this->_state);
}
