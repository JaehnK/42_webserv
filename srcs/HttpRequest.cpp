#include "HttpRequest.hpp"
#include "StringUtils.hpp"

// OCCF RULES
HttpRequest::HttpRequest()
{
    _headersComplete = false;
    _bodyComplete = false;
    _contentLen = 0;
    _bodyReceived = 0;
}

HttpRequest::~HttpRequest()
{
    
}

int HttpRequest::parseRequest(const std::string& rawData)
{
    if (rawData.empty())
        return -1;
    // 헤더가 완료되지 않았다면 헤더 파싱
    if (!_headersComplete) {
        size_t headerEnd = rawData.find("\r\n\r\n");
        if (headerEnd == std::string::npos) {
            // 아직 헤더가 완전하지 않음
            return 0;
        }
        
        std::string headerSection = rawData.substr(0, headerEnd);
        std::vector<std::string> lines = StringUtils::split(headerSection, "\r\n");
        
        if (lines.empty()) {
            return -1;
        }
        
        // 첫 번째 줄: Request Line 파싱
        if (parseRequestLine(lines[0]) != 0) {
            return -1;
        }
        
        // 나머지 줄들: Header 파싱
        for (size_t i = 1; i < lines.size(); ++i) {
            if (parseHeader(lines[i]) != 0) {
                return -1;
            }
        }
        
        _headersComplete = true;
        
        // Content-Length 확인
        std::string content_length_str = getHeader("content-length");
        if (!content_length_str.empty()) {
            _contentLen = static_cast<size_t>(std::atol(content_length_str.c_str()));
        }
        
        // 바디가 있다면 바디 파싱
        if (_contentLen > 0) {
            size_t body_start = headerEnd + 4; // "\r\n\r\n" 길이
            if (body_start < rawData.length()) {
                _body = rawData.substr(body_start);
                _bodyReceived = _body.length();
            }
            _bodyComplete = (_bodyReceived >= _contentLen);
        } else {
            _bodyComplete = true;
        }
    } else {
        // 헤더는 완료, 바디만 추가
        _body += rawData;
        _bodyReceived = _body.length();
        _bodyComplete = (_bodyReceived >= _contentLen);
    }
    
    return 0;
}

int HttpRequest::appendData(const std::string& data) {
    if (!_headersComplete) {
        return parseRequest(data);
    } else {
        _body += data;
        _bodyReceived = _body.length();
        _bodyComplete = (_bodyReceived >= _contentLen);
        return 0;
    }
}

int HttpRequest::parseRequestLine(const std::string& line) {
    std::vector<std::string> parts = StringUtils::split(line, " ");
    if (parts.size() != 3) {
        return -1;
    }
    
    _method = parts[0];
    
    // URL에서 path와 query string 분리
    std::string url = parts[1];
    size_t query_pos = url.find('?');
    if (query_pos != std::string::npos) {
        _path = url.substr(0, query_pos);
        _queryString = url.substr(query_pos + 1);
    } else {
        _path = url;
        _queryString = "";
    }
    
    _version = parts[2];
    
    return 0;
}

int HttpRequest::parseHeader(const std::string& line) {
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        return -1;
    }
    
    std::string key = StringUtils::trim(StringUtils::toLower(line.substr(0, colon_pos)));
    std::string value = StringUtils::trim(line.substr(colon_pos + 1));
    
    _headers[key] = value;
    return 0;
}

std::string HttpRequest::getHeader(const std::string& key) const {
    std::string lower_key = StringUtils::toLower(key);
    std::map<std::string, std::string>::const_iterator it = _headers.find(lower_key);
    return (it != _headers.end()) ? it->second : "";
}

bool HttpRequest::hasHeader(const std::string& key) const {
    std::string lower_key = StringUtils::toLower(key);
    return _headers.find(lower_key) != _headers.end();
}

std::string HttpRequest::getFullUrl() const {
    if (_queryString.empty()) {
        return _path;
    }
    return _path + "?" + _queryString;
}

void HttpRequest::reset() {
    _method.clear();
    _path.clear();
    _queryString.clear();
    _version.clear();
    _headers.clear();
    _body.clear();
    _headersComplete = false;
    _bodyComplete = false;
    _contentLen = 0;
    _bodyReceived = 0;
}