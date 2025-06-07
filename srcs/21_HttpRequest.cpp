#include "webserv.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::HttpRequest(int fd)
{
	char buf[4096];

	this->_fd = fd;
	this->_state = IN_REQUEST;
	this->_headers = std::map<std::string, std::string> ();
	this->_contentLength = 0;  // 초기화 추가
    this->_bodyBytesRead = 0;  // 초기화 추가

	int retryCount = 0;
    const int MAX_RETRIES = 10;  // 최대 재시도 횟수
	while (retryCount < MAX_RETRIES)
	{
		std::memset(buf, 0, 4096);
		this->_bodyBytesRead =  recv(this->_fd, buf, sizeof(buf) - 1, 0);
		
		if (this->_bodyBytesRead < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                retryCount++;
                usleep(10000);  // 10ms 대기
                if (retryCount >= MAX_RETRIES) {
                    // 충분히 기다렸는데도 데이터가 없으면 현재까지 받은 데이터로 처리
                    std::cout << "Timeout waiting for data, processing current buffer" << std::endl;
                    break;
                }
                continue;
            }
			else 
				throw HttpRequestException();
		} 
		else if (this->_bodyBytesRead == 0) 
			break;
		else
		{
			retryCount = 0;  // 데이터를 받으면 재시도 카운터 리셋
			this->_buffer.append(buf, this->_bodyBytesRead);
			processBuffer();
		}

		if (this->_state == COMPLETE)
			break ;
	}
	if (this->_state != COMPLETE && !this->_buffer.empty()) {
        std::cout << "Processing incomplete request with available data" << std::endl;
        processBuffer();
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
    // std::cout << "=== processBuffer START ===" << std::endl;
    // std::cout << "Current state: " << this->_state << std::endl;
    // std::cout << "Buffer content: [" << this->_buffer << "]" << std::endl;
    // std::cout << "Buffer length: " << this->_buffer.length() << std::endl;

	bool _continue = true;

	while (_continue)
	{
		_continue = false;
		switch (this->_state)
		{
			case IN_REQUEST:
			{
                // std::cout << "Processing IN_REQUEST state" << std::endl;
                size_t pos = this->_buffer.find("\r\n");
                // std::cout << "Looking for \\r\\n, position: " << pos << std::endl;
				// if (pos != std::string::npos)
				// 	return ;
				if (pos == std::string::npos)
                {
                    // std::cout << "No \\r\\n found, returning" << std::endl;
                    return;
                }
				std::string	buf = this->_buffer.substr(0, pos);
				// std::cout << "Extracted request line: [" << buf << "]" << std::endl;
				// if (!parseRequest(buf))
				// 	throw HttpRequestSyntaxException();
				parseRequest(buf);
				_continue = true;
				_state = IN_HEADER;
				this->_buffer.erase(0, pos + 2);
				break;
			}

			case IN_HEADER:
			{
				// std::cout << "Processing IN_HEADER state" << std::endl;
				// std::cout << "Buffer before processing: [" << this->_buffer.substr(0, 50) << "...]" << std::endl;
    
				size_t pos = this->_buffer.find("\r\n");
				// std::cout << "Looking for header \\r\\n, position: " << pos << std::endl;
				if (pos == std::string::npos)
					return ;
				else if (pos == 0)
				{
					// std::cout << "Found empty line, moving to IN_EMPTYLINE" << std::endl;
					_continue = true;
					this->_buffer.erase(0, 2);
					this->_state = IN_EMPTYLINE;
					// std::cout << "Buffer after empty line erase: [" << this->_buffer.substr(0, 50) << "...]" << std::endl;
				}	
				else
				{
					std::string buf = this->_buffer.substr(0, pos);
					// std::cout << "Processing header: [" << buf << "]" << std::endl;
					// std::cout << "About to erase " << (pos + 2) << " characters" << std::endl;
					if (!parseHeaders(buf))
					throw HttpRequestSyntaxException();
				
					this->_buffer.erase(0, pos + 2);
					// std::cout << "Buffer after header erase: [" << this->_buffer.substr(0, 50) << "...]" << std::endl;
					_continue = true;
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

bool HttpRequest::parseHeaders(const std::string &buf)
{
    size_t valuePos = buf.find(": ");
    if (valuePos == std::string::npos)
        return false;
    
    std::string key = buf.substr(0, valuePos);
    std::string value = buf.substr(valuePos + 2);
    
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    this->_headers[key] = value;
    
    if (key == "Content-Length")
        this->_contentLength = std::atoi(value.c_str());
    else if (key == "Content-Type")
        this->_contentType = value;
    
    return true;
}

bool HttpRequest::parseAllHeaders(const std::string &headerBlock)
{
    size_t position = 0;
    
    while (position < headerBlock.length())
    {
        size_t newlinePos = headerBlock.find("\r\n", position);
        if (newlinePos == std::string::npos)
            newlinePos = headerBlock.length();
        
        std::string line = headerBlock.substr(position, newlinePos - position);
        
        if (line.empty())
            break;
        
        // 단일 헤더 라인 파싱
        size_t valuePos = line.find(": ");
        if (valuePos == std::string::npos)
            return false;
        
        std::string key = line.substr(0, valuePos);
        std::string value = line.substr(valuePos + 2);
        
        // 공백 제거
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        this->_headers[key] = value;
        
        // 특별한 헤더들 처리
        if (key == "Content-Length")
            this->_contentLength = std::atoi(value.c_str());
        else if (key == "Content-Type")
            this->_contentType = value;
        
        position = newlinePos + 2;
    }
    
    return true;
}

void	HttpRequest::parseRequest(const std::string &buf)
{
	// std::cout << "Parsing request line: [" << buf << "]" << std::endl;  // 추가
	std::vector<std::string> elements;
	
	elements = ServerManager::split(buf, ' ');
	// std::cout << "Split elements count: " << elements.size() << std::endl;  // 추가
	for (size_t i = 0; i < elements.size(); i++) {
        std::cout << "Element[" << i << "]: [" << elements[i] << "]" << std::endl;  // 추가
    }
	if (elements.size() < 2) {  // 검증 추가
        // std::cout << "Not enough elements in request line" << std::endl;
		this->_method = GET;      // 기본값 설정
        this->_url = "/";                // 기본값 설정
        return;
        // throw HttpRequestSyntaxException();
    }
	if (elements.at(0) == "GET")
		this->_method = GET;
	else if (elements.at(0) == "POST")
		this->_method = POST;
	else if (elements.at(0) == "DELETE")
		this->_method = DELETE;
	else
    {
        std::cout << "Unknown method: [" << elements.at(0) << "]" << std::endl;  // 추가
		this->_method = GET;      // 기본값 설정
        // throw HttpRequestSyntaxException();
    }
	this->_url = elements.at(1);
    if (this->_url.empty()) {
        this->_url = "/";                // 빈 URL 방지
    }
    std::cout << "Parsed URL: [" << this->_url << "]" << std::endl;  // 추가
}

const char *HttpRequest::HttpRequestException::what() const throw()
{
	return ("You MotherFucker");
}

const char *HttpRequest::HttpRequestSyntaxException::what() const throw()
{
	return ("You MotherFucker");
}