#include "webserv.hpp"
std::map<int, std::string> HttpResponse::_statMsgs;
std::map<std::string, std::string> HttpResponse::_mimeTypes;
HttpResponse::HttpResponse()
{}

HttpResponse::HttpResponse(HttpRequest& req)
{
    initStaticVars();
    _reqMethod = req.getMethod();
    _reqUrl = req.getUrl();
    _statCode = 0;
}

HttpResponse::HttpResponse(const HttpResponse& rhs)
{
	*this = rhs;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& rhs)
{
    if (this != &rhs)
	{
		this->_reqMethod = rhs.getReqMethod();
        this->_reqUrl = rhs.getReqUrl();
        this->_statCode = rhs.getStatCode();
        this->_mimeType = rhs.getMimeType();
        this->_body = rhs.getBody();
        this->_totalResp = rhs.getTotalResp();
        initStaticVars();
	}
	return (*this);
}

HttpResponse::~HttpResponse()
{
    
}

HttpMethod  HttpResponse::getReqMethod() const
{
    return (this->_reqMethod);
}

std::string HttpResponse::getReqUrl() const
{
    return (this->_reqUrl);
}

int         HttpResponse::getStatCode() const
{
    return (this->_statCode);
}

std::string HttpResponse::getMimeType() const
{
    return (this->_mimeType);
}

std::string HttpResponse::getBody() const
{
    return (this->_body);
}

std::string HttpResponse::getTotalResp() const
{
    return (this->_totalResp);
}

std::string HttpResponse::getStatMsg(int code) const
{
    return (this->_statMsgs[code]);
}

std::string HttpResponse::getContentType(std::string mime) const
{
    std::map<std::string, std::string>::iterator it;
    
    it = _mimeTypes.find(mime);
    if (it != _mimeTypes.end())
    {
        return (it->second);
    }
    return ("application/octet-stream");
}

void    HttpResponse::initStaticVars()
{
    static bool _initalised = false;
    if (!_initalised)
    {
        _statMsgs[200] = "OK";
        _statMsgs[201] = "Created";
        _statMsgs[400] = "Bad Request";
        _statMsgs[401] = "Unauthorized";
        _statMsgs[403] = "Forbidden";
        _statMsgs[404] = "Not Found";
        _statMsgs[405] = "Method Not Allowed";
        _statMsgs[500] = "Internal Server Error";
        _statMsgs[502] = "Bad Gateway";
        _statMsgs[503] = "Service Unavailable";
        _mimeTypes[".css"] = "text/css";
        _mimeTypes[".js"] = "application/javascript";
        _mimeTypes[".html"] = "text/html";
        _mimeTypes[".htm"] = "text/html";
        _mimeTypes[".png"] = "image/png";
        _mimeTypes[".jpg"] = "image/jpeg";
        _mimeTypes[".jpeg"] = "image/jpeg";
        _mimeTypes[".gif"] = "image/gif";
        _mimeTypes[".ico"] = "image/x-icon";
        _mimeTypes[".txt"] = "text/plain";
        _mimeTypes[".pdf"] = "application/pdf";
        _mimeTypes[".json"] = "application/json";
        _initalised = true;
    }
}

bool    HttpResponse::checkAllowedMethod(std::vector<std::string> limits)
{
    if (limits.empty())
        return true;
    std::string methodStr;
    switch (this->_reqMethod)
    {
        case GET:    
            methodStr = "GET"; 
            break;
        case POST:  
            methodStr = "POST";
            break;
        case DELETE: 
            methodStr = "DELETE";
            break;
        default: 
            _statCode = 405;
            _body = "<!DOCTYPE html><html><body><h1>405 Method Not Allowed</h1></body></html>";
            return false;
    }
    
    bool check = std::find(limits.begin(), limits.end(), methodStr) != limits.end();
    if (!check)
    {
        _statCode = 405;
        _body = "<!DOCTYPE html><html><body><h1>405 Method Not Allowed</h1></body></html>";
    }
    return check;
}

void    HttpResponse::handleMethod()
{
    switch  (this->_reqMethod)
    {
        case GET:
            createGetResponse();
            break;

        case POST:
            createPostResponse();
            break;

        case DELETE:
            createDeleteResponse();
            break;
    }
}

void    HttpResponse::createGetResponse()
{
    std::string     path;
    std::string     filePath;
    
    path = HttpServer::extractPath(this->_reqUrl);
    if (path == "/")
        path = "/index.html";
    filePath = "./www" + path;
    std::ifstream   file(filePath.c_str());
    // 갈라치기 예정
    if (file.good())
    {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        this->_body = buffer.str();
        file.close();
        this->_statCode = 200;
        size_t dotPos = filePath.find_last_of('.');
        if (dotPos != std::string::npos) 
        {
            std::string extension = filePath.substr(dotPos);
            this->_mimeType = extension;
        }

    }
    else
    {
        this->_statCode = 404;
        this->_mimeType = ".html";
        this->_body = "Not Found";
    }
}


void    HttpResponse::createPostResponse()
{}
void    HttpResponse::createDeleteResponse()
{}

void    HttpResponse::assembleMsg()
{
    std::ostringstream msg;
    
    msg << "HTTP/1.1 " << this->_statCode << " " << getStatMsg(this->_statCode) << "\r\n";
    msg << "Content-Type: " << getContentType(this->_mimeType) << "\r\n";
    msg << "Content-Length: " << _body.length() << "\r\n";
    msg << "\r\n";
    msg << _body;

    this->_totalResp = msg.str();
}