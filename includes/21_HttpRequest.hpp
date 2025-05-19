#pragma once
#include "webserv.hpp"

enum HttpMethod
{
    METHOD_GET,
    METHOD_POST,
    METHOD_DELETE,
};

class HttpRequest
{
    private:
		
		enum	reqState
		{
			IN_REQEUST,
			IN_HEADER,
			IN_EMPTYLINE,
			IN_BODY,
			COMPLETE,
			ERROR
		};

		int									_fd;
		HttpMethod							_method;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		reqState							_state;


        HttpRequest();
    public:
	
        HttpRequest(int fd);
		HttpRequest(const HttpRequest& rhs);
		HttpRequest&	operator=(const HttpRequest &rhs);
        ~HttpRequest();

		// setter
		void	setMethod(const std::string& method);
		
		// getter


};
