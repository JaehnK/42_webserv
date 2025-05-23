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
	IN_REQUEST,
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
		std::string							_url;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		std::string							_contentType;
		size_t 								_contentLength;              
		int 								_bodyBytesRead;
		ReqState							_state;

		HttpRequest();
		void	processBuffer();
		void	parseRequest(const std::string& buf);
		bool	parseHeaders(const std::string& buf);
		bool	parseAllHeaders(const std::string &headerBlock);

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
		std::string							getUrl() const;
		std::string							getBody() const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string							getContentType() const;
		size_t								getContentLength() const;
		size_t								getBodyBytesRead() const;
		ReqState							getState() const;

		class HttpRequestException: public std::exception
		{
			const char *what() const throw();
		};

		class HttpRequestSyntaxException: public std::exception
		{
			const char *what() const throw();
		};
};
