#include "webserv.hpp"

LocationCGI::LocationCGI()
{
    this->_param = std::vector<std::string> ();
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
            this->_param = std::vector<std::string> ();
        }
    }
    return (*this);
}

LocationCGI::~LocationCGI()
{

}

void    LocationCGI::setPass(std::string pass)
{
    this->_pass = pass;
}
void    LocationCGI::setIncludeParam(bool param)
{
    this->_includeParam = param;
}
void    LocationCGI::setIncludePass(bool pass)
{
    this->_includePass = pass;
}
void    LocationCGI::addParam(std::string param)
{
    this->_param.push_back(param);
}

// getter
std::string                 LocationCGI::getPass() const
{
    if (this->hasPass() == false)
        throw DataNotFoundException();
    return (this->_pass);
}

std::vector<std::string>    LocationCGI::getParam() const
{
    if (this->hasParam() == false)
        throw DataNotFoundException();
    return (this->_param);
}

bool    LocationCGI::getIncludePass() const
{
    return (this->_includePass);
}

bool    LocationCGI::getIncludeParam() const
{
    return (this->_includeParam);
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
