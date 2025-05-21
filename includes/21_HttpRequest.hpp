#pragma once
#include "webserv.hpp"

enum	HttpMethod
{
    METHOD_GET,
    METHOD_POST,
    METHOD_DELETE,
};

enum	ReqState
{
	IN_REQEUST,
	IN_HEADER,
	IN_EMPTYLINE,
	IN_BODY,
	COMPLETE,
};

class HttpRequest
{
    private:

		int									_fd;
		std::string							_buffer;
		HttpMethod							_method;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		size_t 								_contentLength;              
		int 								_bodyBytesRead;
		ReqState							_state;

		HttpRequest();
		void	processBuffer();

		 
	public:
        HttpRequest(int fd);
		HttpRequest(const HttpRequest& rhs);
		HttpRequest&	operator=(const HttpRequest &rhs);
        ~HttpRequest();

		// setter
		void	setMethod(HttpMethod method);
		void	setBody(const std::string& body);
		void	setContentLength(size_t length);
		void	setBodyBytesRead(size_t readBytes);
		void	addHeader(const std::string& key, const std::string& value);
		
		// getter
		int									getFd() const;
		HttpMethod							getMethod() const;
		std::string							getBody() const;
		std::map<std::string, std::string>	getHeaders() const;
		size_t								getContentLength() const;
		size_t								getBodyBytesRead() const;
		ReqState							getState() const;

		class HttpRequestException: public std::exception
		{
			const char *what() const throw();
		};
};
