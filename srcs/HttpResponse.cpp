#include "HttpResponse.hpp"
#include <sstream>
#include <fstream>

HttpResponse::HttpResponse() : _statusCode(200), _headersSent(false), _bodySet(false) {
    _statusMessage = getStatusMessage(200);
    setHeader("Server", "WebServer/1.0");
    setHeader("Date", getCurrentDateTime());
}

void HttpResponse::setStatus(int code) {
    _statusCode = code;
    _statusMessage = getStatusMessage(code);
}

void HttpResponse::setStatus(int code, const std::string& message) {
    _statusCode = code;
    _statusMessage = message;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

void HttpResponse::removeHeader(const std::string& key) {
    _headers.erase(key);
}

std::string HttpResponse::getHeader(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    return (it != _headers.end()) ? it->second : "";
}

bool HttpResponse::hasHeader(const std::string& key) const {
    return _headers.find(key) != _headers.end();
}

void HttpResponse::setBody(const std::string& content) {
    _body = content;
    _bodySet = true;
    setContentLength(_body.length());
}

void HttpResponse::appendBody(const std::string& content) {
    _body += content;
    _bodySet = true;
    setContentLength(_body.length());
}

void HttpResponse::clearBody() {
    _body.clear();
    _bodySet = false;
    removeHeader("Content-Length");
}

int HttpResponse::setFile(const std::string& filepath) {
    // 파일 읽기 (실제 구현에서는 FileUtils 사용)
    // 여기서는 간단한 예시
    std::ifstream file(filepath.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return -1;
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    _body = oss.str();
    _bodySet = true;
    setContentLength(_body.length());
    
    // MIME 타입 설정 (간단한 예시)
    if (filepath.find(".html") != std::string::npos) {
        setContentType("text/html");
    } else if (filepath.find(".css") != std::string::npos) {
        setContentType("text/css");
    } else if (filepath.find(".js") != std::string::npos) {
        setContentType("application/javascript");
    } else if (filepath.find(".png") != std::string::npos) {
        setContentType("image/png");
    } else if (filepath.find(".jpg") != std::string::npos || filepath.find(".jpeg") != std::string::npos) {
        setContentType("image/jpeg");
    } else {
        setContentType("application/octet-stream");
    }
    
    return 0;
}

void HttpResponse::setContentType(const std::string& type) {
    setHeader("Content-Type", type);
}

void HttpResponse::setContentLength(size_t length) {
    std::ostringstream oss;
    oss << length;
    setHeader("Content-Length", oss.str());
}

void HttpResponse::setRedirect(int code, const std::string& location) {
    setStatus(code);
    setHeader("Location", location);
}

void HttpResponse::setErrorPage(int code, const std::string& error_page_path) {
    setStatus(code);
    
    if (!error_page_path.empty() && setFile(error_page_path) == 0) {
        // 에러 페이지 파일 로드 성공
        return;
    }
    
    // 기본 에러 페이지 생성
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n"
        << "<html><head><title>" << code << " " << _statusMessage << "</title></head>\n"
        << "<body><h1>" << code << " " << _statusMessage << "</h1></body></html>";
    
    setBody(oss.str());
    setContentType("text/html");
}

void HttpResponse::setKeepAlive(bool enable, int timeout) {
    if (enable) {
        setHeader("Connection", "keep-alive");
        std::ostringstream oss;
        oss << "timeout=" << timeout;
        setHeader("Keep-Alive", oss.str());
    } else {
        setHeader("Connection", "close");
        removeHeader("Keep-Alive");
    }
}

std::string HttpResponse::toString() const {
    std::ostringstream oss;
    
    // Status Line
    oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
    
    // Headers
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); 
         it != _headers.end(); ++it) {
        oss << it->first << ": " << it->second << "\r\n";
    }
    
    // Empty line
    oss << "\r\n";
    
    // Body
    oss << _body;
    
    return oss.str();
}

std::string HttpResponse::getHeaderString() const {
    std::ostringstream oss;

    // Status Line
    oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";

    // Headers
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); 
         it != _headers.end(); ++it) {
        oss << it->first << ": " << it->second << "\r\n";
    }

    // Empty line
    oss << "\r\n";

    return oss.str();
}

int HttpResponse::getStatusCode() const { return _statusCode; }
const std::string&  HttpResponse::getStatusMessage() const { return _statusMessage; }
const std::string&  HttpResponse::getBody() const { return _body; }
size_t HttpResponse::getContentLength() const { return _body.length(); }

void HttpResponse::reset() {
    _statusCode = 200;
    _statusMessage = getStatusMessage(200);
    _headers.clear();
    _body.clear();
    _headersSent = false;
    _bodySet = false;
    
    setHeader("Server", "WebServer/1.0");
    setHeader("Date", getCurrentDateTime());
}

std::string HttpResponse::getStatusMessage(int code) const {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

std::string HttpResponse::getCurrentDateTime() const {
    time_t now = time(0);
    struct tm* timeinfo = gmtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return std::string(buffer);
}
