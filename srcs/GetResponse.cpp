#include "GetResponse.hpp"
#include <sys/stat.h>

GetResponse::GetResponse(const HttpRequest& req) : _req(req), _statusCode(500)
{

}

GetResponse::GetResponse(const GetResponse& obj)
{
    *this = obj;
}

GetResponse& GetResponse::operator=(const GetResponse& obj)
{
    if (this != &obj)
    {

    }
    return *this;
}

GetResponse::~GetResponse()
{
    
}

void    GetResponse::handleRequest()
{
    std::string urlPath = _req.getUrl();
    std::string filePath = "./www" + urlPath;

    if (filePath.back() == '/')
        filePath += "index.html";

    struct stat statbuf;
    if (stat(filePath.c_str(), &statbuf) != 0 || S_ISDIR(statbuf.st_mode))
    {
        _statusCode = 404;
        _body = "<html><body><h1>404 Not Found</h1></body></html>";
        _mimeType = "text/html";
        assembleResponse();
        return ;
    }

    std::fstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file)
    {
        _statusCode = 500;
        _body = "<html><body><h1>500 Internal Server Error</h1></body></html>";
        _mimeType = "text/html";
        assembleResponse();
        return ;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    _body = ss.str();
    _statusCode = 200;

    size_t extPos = filePath.find_last_of('.');
    if (extPos != std::string::npos) {
        std::string ext = filePath.substr(extPos + 1);
        _mimeType = getMimeType(ext);
    } else {
        _mimeType = "application/octet-stream";
    }
    assembleResponse();
}

std::string GetResponse::getTotalResp() const
{
    
}


void    GetResponse::assembleResponse()
{
    std::ostringstream res;

    res << "HTTP/1.1 " << _statusCode << " " << getSatusText(_statusCode) << "\r\n";
    res << "Content-Type: " << _mimeType << "\r\n";
    res << "Content-Length: " << _body.size() << "\r\n";
    res << "Connection: close\r\n";
    res << "\r\n";
    res << _body;

    _response = res.str();
}

std::string GetResponse::getMimeType(const std::string& ext) const {
    static std::map<std::string, std::string> mimeTypes = {
        {"html", "text/html"},
        {"htm", "text/html"},
        {"css", "text/css"},
        {"js", "application/javascript"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"png", "image/png"},
        {"gif", "image/gif"},
        {"txt", "text/plain"},
        {"json", "application/json"}
    };

    std::map<std::string, std::string>::iterator it = mimeTypes.find(ext);
    return (it != mimeTypes.end()) ? it->second : "application/octet-stream";
}

std::string GetResponse::getSatusText(int code) const
{
    switch (code) {
        case 200: return "OK";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default:  return "Unknown";
    }
}
