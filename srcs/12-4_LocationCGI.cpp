#include "webserv.hpp"

LocationCGI::LocationCGI()
{
    this->_locType = CGI;
    this->_param = std::map<std::string, std::string>  ();
    initParam();
}

LocationCGI::LocationCGI(const LocationCGI& rhs): Location(rhs)
{
    *this = rhs;
}

LocationCGI& LocationCGI::operator=(const LocationCGI& rhs)
{
    if (this != &rhs)
    {
        Location::operator=(rhs);
        
        try
        {
            this->_pass = rhs.getPass();
        }
        catch(const std::exception& e)
        {
            this->_pass.clear();
        }
        try
        {
            this->_param = rhs.getParam();
        }
        catch(const std::exception& e)
        {
            this->_param = std::map<std::string, std::string>  ();
        }
    }
    return (*this);
}

LocationCGI::~LocationCGI()
{

}

void LocationCGI::initParam() 
{
    this->_param["SCRIPT_FILENAME"] = "$document_root$fastcgi_script_name";
    this->_param["QUERY_STRING"] = "$query_string";
    this->_param["REQUEST_METHOD"] = "$request_method";
    this->_param["CONTENT_TYPE"] = "$content_type";
    this->_param["CONTENT_LENGTH"] = "$content_length";
    this->_param["SCRIPT_NAME"] = "$fastcgi_script_name";
    this->_param["REQUEST_URI"] = "$request_uri";
    this->_param["DOCUMENT_URI"] = "$document_uri";
    this->_param["DOCUMENT_ROOT"] = "$document_root";
    this->_param["SERVER_PROTOCOL"] = "$server_protocol";
    this->_param["REQUEST_SCHEME"] = "$scheme";
    this->_param["HTTPS"] = "$https if_not_empty";
    this->_param["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_param["SERVER_SOFTWARE"] = "nginx/$nginx_version";
    this->_param["REMOTE_ADDR"] = "$remote_addr";
    this->_param["REMOTE_PORT"] = "$remote_port";
    this->_param["SERVER_ADDR"] = "$server_addr";
    this->_param["SERVER_PORT"] = "$server_port";
    this->_param["SERVER_NAME"] = "$server_name";
}

void    LocationCGI::setPass(std::string pass)
{
    this->_pass = pass;
}

void    LocationCGI::setcgiIndex(std::string idx)
{
    this->_cgiIndex = idx;
}

void    LocationCGI::addParam(std::string name, std::string value)
{
    this->_param[name] = value;
}

// getter
std::string  LocationCGI::getPass() const
{
    if (this->hasPass() == false)
        throw DataNotFoundException();
    return (this->_pass);
}

std::string LocationCGI::getCgiIndx() const
{
    return (this->_cgiIndex);
}


std::map<std::string, std::string>    LocationCGI::getParam() const
{
    if (this->hasParam() == false)
        throw DataNotFoundException();
    return (this->_param);
}

bool    LocationCGI::hasPass() const
{
    if (this->_pass.empty())
        return (false);
    return (true);
}

bool    LocationCGI::hasParam() const
{
    if (this->_param.size() == 0)
        return (false);
    return (true);
}
