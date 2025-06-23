#pragma once

#include <string>
#include <map>
#include <vector>

class   HttpRequest
{
    private:
        std::string                         _method;
        std::string                         _path;
        std::string                         _queryString;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;

        bool    _headersComplete;
        bool    _bodyComplete;
        size_t  _contentLen;
        size_t  _bodyReceived;

        int parseRequestLine(const std::string& line);
        int parseHeader(const std::string& line);
    public:
        HttpRequest();
        ~HttpRequest();

        int parseRequest(const std::string& raw_data);
        int appendData(const std::string& data);

        // Getters
        const std::string& getMethod() const { return _method; }
        const std::string& getPath() const { return _path; }
        const std::string& getQueryString() const { return _queryString; }
        const std::string& getVersion() const { return _version; }
        const std::string& getBody() const { return _body; }

        std::string getHeader(const std::string& key) const;
        bool hasHeader(const std::string& key) const;
        const std::map<std::string, std::string>& getHeaders() const { return _headers; }

        // 상태 확인
        bool isHeadersComplete() const { return _headersComplete; }
        bool isBodyComplete() const { return _bodyComplete; }
        bool isComplete() const { return _headersComplete && _bodyComplete; }
        bool hasBody() const { return _contentLen > 0; }

        // 유틸리티
        size_t getContentLength() const { return _contentLen; }
        size_t getBodyReceived() const { return _bodyReceived; }
        std::string getFullUrl() const;

        // 디버깅
        void    reset();
};