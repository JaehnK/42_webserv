#include "webserv.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::HttpRequest(int fd)
{
	char buf[4096];

	this->_fd = fd;
	this->_state = IN_REQEUST;
	this->_headers = std::map<std::string, std::string> ();
	
	while (1)
	{
		std::memset(buf, 0, 4096);
		this->_bodyBytesRead =  recv(this->_fd, buf, sizeof(buf) - 1, 0);
		
		if (this->_bodyBytesRead < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				throw HttpRequestException();
			else 
				throw HttpRequestException();
		} 
		else if (this->_bodyBytesRead == 0) 
			break;
		else
		{
			this->_buffer.append(buf);
			processBuffer();
		}

		if (this->_state == COMPLETE)
			break ;
	}
}

HttpRequest::HttpRequest(const HttpRequest& rhs)
{
	*this = rhs;
}

HttpRequest&    HttpRequest::operator=(const HttpRequest& rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs.getFd();
		this->_method = rhs.getMethod();
		this->_url = rhs.getUrl();
		this->_body = rhs.getBody();
		this->_headers = rhs.getHeaders();
		this->_contentType = rhs.getContentType();
		this->_contentLength = rhs.getContentLength();
		this->_bodyBytesRead = rhs.getBodyBytesRead();
		this->_state = rhs.getState();
	}
	return (*this);
}

HttpRequest::~HttpRequest()
{}

void	HttpRequest::setMethod(HttpMethod method)
{
	this->_method = method;
}

void	HttpRequest::setBody(const std::string& body)
{
	this->_body = body;
}

void	HttpRequest::setContentLength(size_t length)
{
	this->_contentLength = length;
}

void	HttpRequest::setBodyBytesRead(size_t readBytes)
{
	this->_bodyBytesRead = readBytes;
}

void	HttpRequest::addHeader(const std::string& key, const std::string& value)
{
	this->_headers[key] = value;
}

int HttpRequest::getFd() const
{
	return (this->_fd);
}

HttpMethod	HttpRequest::getMethod() const
{
	return (this->_method);
}

std::string	HttpRequest::getUrl() const
{
	return (this->_url);
}

std::string	HttpRequest::getBody() const
{
	return (this->_body);
}

std::map<std::string, std::string>	HttpRequest::getHeaders() const
{
	return (this->_headers);
}

std::string	HttpRequest::getContentType() const
{
	return (this->_contentType);
}

size_t	    HttpRequest::getContentLength() const
{
	return (this->_contentLength);
}

size_t	    HttpRequest::getBodyBytesRead() const
{
	return (this->_bodyBytesRead);
}

ReqState    HttpRequest::getState() const
{
	return (this->_state);
}

void    HttpRequest::processBuffer()
{
	bool _continue = false;

	while (_continue)
	{
		if (_continue)
			_continue = false;

		switch (this->_state)
		{
			case IN_REQEUST:
			{
				size_t pos = this->_buffer.find("\r\n");
				if (pos != std::string::npos)
					return ;
				
				std::string	buf = this->_buffer.substr(0, pos);
				if (!parseRequest(buf))
					throw HttpRequestSyntaxException();
				
				_continue = true;
				_state = IN_HEADER;
				this->_buffer.erase(0, pos + 2);
				break;
			}

			case IN_HEADER:
			{
				size_t pos = this->_buffer.find("\r\n");
				if (pos == std::string::npos)
					return ;
				else if (pos == 0)
				{
					_continue = true;
					this->_buffer.erase(0, 2);
					this->_state = IN_EMPTYLINE;
				}	
				else
				{
					std::string	buf = this->_buffer.substr(0, pos);
					if (!parseHeaders(buf))
						throw HttpRequestSyntaxException();
					_continue = true;
					this->_buffer.erase(0, 2);
				}
				break;
			}

			case IN_EMPTYLINE:
			{
				_continue = true;
				this->_state = IN_BODY;
				break;
			}

			case IN_BODY:
			{
				if (this->_contentLength)
				{
					if (this->_buffer.size() >= this->_contentLength)
					{
						this->_body = this->_buffer.substr(0, this->_contentLength);
						this->_buffer.erase(0, this->_contentLength);
						this->_state = COMPLETE;
					}
					else
						return ;
				}
				else
				{
					this->_state = COMPLETE;
					return ;
				}
				break;
			}
			
			case COMPLETE:
				break;
		}
	}
}

void	HttpRequest::parseHeaders(const std::string &buf)
{
	size_t		position = 0;
	size_t		newlinePos, valuePos;
	std::string	temp = buf;
	std::string	key, value;
	
	while ((newlinePos = temp.find("\r\n")))
	{
		std::string	line = temp.substr(position, newlinePos - position);
		if (line.empty())
			break;

		valuePos = line.find(": ");
		if (valuePos == std::string::npos)
			return (false);
		
		key = line.substr(0, pos);
		value = line.substr(pos + 2, std::string::npos);
		this->_headers[key] = value;
		
		if (key == "Content-Length")
			this->_contentLength = std::atoi(value.c_str());
		else if (key == "Content-Type")
			this->_contentType = value;
		
		position = newlinePos + 2;
	}
	return (true);
}

void	HttpRequest::parseRequest(const std::string &buf)
{
	std::vector<std::string> elements;
	
	elements = ServerManager::split(buf, ' ');
	
	if (elements.at(0) == "GET")
		this->_method = METHOD_GET;
	else if (elements.at(0) == "POST")
		this->_method = METHOD_POST;
	else if (elements.at(0) == "DELETE")
		this->_method = METHOD_DELETE;
	else
		throw HttpRequestSyntaxException();
	
	this->_url = elements.at(1);

}

const char *HttpRequest::HttpRequestException::what() const throw()
{
	return ("You MotherFucker");
}

const char *HttpRequest::HttpRequestSyntaxException::what() const throw()
{
	return ("You MotherFucker");
}