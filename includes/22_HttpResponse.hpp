#pragma once
#include "webserv.hpp"

class HttpResponse
{
    private:
        HttpMethod      _reqMethod;
        std::string     _reqUrl;
        int             _statCode;
        std::string     _mimeType;
        std::string     _body;
        std::string		_totalResp;
        
        static bool                                 _initalised;
        static std::map<int, std::string>           _statMsgs;
        static std::map<std::string, std::string>   _mimeTypes;
        HttpResponse();
    
    public:
        HttpResponse(HttpRequest& req);
        HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse &rhs);
		~HttpResponse();
        
        static void    initStaticVars();
        
        HttpMethod  getReqMethod() const;
        std::string getReqUrl() const;
        int         getStatCode() const;
        std::string getMimeType() const;
        std::string getBody() const;
        std::string getTotalResp() const;

        std::string getStatMsg(int code) const;
        std::string getContentType(std::string mime) const;

        bool    checkAllowedMethod(std::vector<std::string> limits);
        void    handleMethod();
        void    createGetResponse();
        void    createPostResponse();
        void    createDeleteResponse();


        void    assembleMsg();
        
};
