#pragma once

#include <string>
#include <map>

class HttpResponse {
    private:
        int                                 _statusCode;
        std::string                         _statusMessage;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        
        // 상태
        bool    _headersSent;
        bool    _bodySet;
        
        // 유틸리티 메서드들
        std::string getStatusMessage(int code) const;
        std::string getCurrentDateTime() const;

    public:
        HttpResponse();
        ~HttpResponse();

        // 상태 설정
        void setStatus(int code);
        void setStatus(int code, const std::string& message);

        // 헤더 설정
        void setHeader(const std::string& key, const std::string& value);
        void removeHeader(const std::string& key);
        std::string getHeader(const std::string& key) const;
        bool hasHeader(const std::string& key) const;

        // 바디 설정
        void setBody(const std::string& content);
        void appendBody(const std::string& content);
        void clearBody();

        // 파일 응답
        int setFile(const std::string& filepath);

        // 편의 메서드들
        void setContentType(const std::string& type);
        void setContentLength(size_t length);
        void setRedirect(int code, const std::string& location);
        void setErrorPage(int code, const std::string& error_page_path = "");
        void setKeepAlive(bool enable, int timeout = 60);

        // 응답 생성
        std::string toString() const;
        std::string getHeaderString() const;

        // Getters
        int                 getStatusCode() const;
        const std::string&  getStatusMessage() const;
        const std::string&  getBody() const;
        size_t              getContentLength() const;

        // 상태 확인
        bool    isHeadersSent() const { return _headersSent; }
        bool    isBodySet() const { return _bodySet; }

        // 디버깅
        void    reset();
};