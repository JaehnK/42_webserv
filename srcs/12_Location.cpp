#include "webserv.hpp"

Location::Location()
{
    this->_clientMaxBodySize = -1;
    this->_index = std::vector<std::string> ();
    this->_limitExcept = std::vector<std::string> ();
    this->_errorPage = std::vector<std::map<int, std::string> > ();
    this->_denyAll = true;
}

Location::Location(const Location& rhs)
{
    *this = rhs;
}

Location&   Location::operator=(const Location& rhs)
{
    if (this != &rhs)
    {
        this->_path = rhs.getPath();
        this->_clientMaxBodySize = rhs.getClientMaxBodySize();
        this->_root = rhs.getRoot();
        this->_index = rhs.getIndex();
        this->_limitExcept = rhs.getLimitExcept();
        this->_errorPage = rhs.getErrorPage();
        this->_denyAll = rhs.getDenyAll();
    }
    return (*this);
}

Location::~Location()
{

}


void    Location::setPath(std::string path)
{
    this->_path = path;
}

void    Location::setClientMaxBodySize(int cmbs)
{
    this->_clientMaxBodySize = cmbs;
}

void    Location::setRoot(std::string root)
{
    this->_root = root;
}

void    Location::setDenyall(bool denyall)
{
    this->_denyAll = denyall;
}

void    Location::addIndex(std::string idx)
{
    this->_index.push_back(idx);
}

void    Location::addLimitExcept(std::string limitExcept)
{
    this->_limitExcept.push_back(limitExcept);
}

void    Location::addErrorPage(std::map<int, std::string> errPage)
{
    this->_errorPage.push_back(errPage);
}

std::string Location::getPath() const
{
    if (this->hasPath() == false)
        throw DataNotFoundException();
    return (this->_path);
}

int Location::getClientMaxBodySize() const
{
    if (this->hasClientMaxBodysize() == false)
        throw DataNotFoundException();
    return (this->_clientMaxBodySize);
}

std::string Location::getRoot() const
{
    if (this->hasRoot() == false)
        throw DataNotFoundException();
    return (this->_root);
}

std::vector<std::string>    Location::getIndex() const
{
    if (this->hasIndex() == false)
        throw DataNotFoundException();
    return (this->_index);
}

std::vector<std::string>    Location::getLimitExcept() const
{
    if (this->hasLimitExcept() == false)
        throw DataNotFoundException();
    return (this->_limitExcept);
}

std::vector<std::map<int, std::string> >    Location::getErrorPage() const
{
    if (this->hasErrorPage() == false)
        throw DataNotFoundException();
    return (this->_errorPage);
}

bool    Location::getDenyAll() const
{
    return (this->_denyAll);
}

bool    Location::hasPath() const
{
    if (this->_path.empty())
        return (false);
    return (true);
}

bool    Location::hasClientMaxBodysize() const
{
    if (this->_clientMaxBodySize == -1)
        return (false);
    return (true);
}

bool    Location::hasRoot() const
{
    if (this->_root.empty())
        return (false);
    return (true);
}

bool    Location::hasIndex() const
{
    if (this->_index.size() == 0)
        return (false);
    return (true);
}

bool    Location::hasLimitExcept() const
{
    if (this->_limitExcept.size() == 0)
        return (false);
    return (true);
}

bool    Location::hasErrorPage() const
{
    if (this->_errorPage.size() == 0)
        return (false);
    return (true);
}

const char* Location::DataNotFoundException::what() const throw()
{
    return ("Data does not exist.");
}

