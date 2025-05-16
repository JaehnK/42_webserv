#include "webserv.hpp"

Location::Location()
{
    this->_locType = DEFAULT;
    this->_clientMaxBodySize = -1;
    this->_limitExcept = std::vector<std::string> ();
    this->_errorPage = std::map<int, std::string> ();
    this->_locations = std::vector<Location*> ();
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
    }
    return (*this);
}

Location::~Location()
{
    if (this->_locations.size() > 0)
    {
        for (std::vector<Location *>::iterator it = _locations.begin();
                it != _locations.end(); it++)
            delete *it;
    }
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

void    Location::setIndex(std::string idx)
{
    this->_index = idx;
}

void    Location::addLimitExcept(std::string limitExcept)
{
    this->_limitExcept.push_back(limitExcept);
}

void    Location::addErrorPage(int key, std::string value)
{
    this->_errorPage[key] = value;
}

void    Location::addLocations(Location* loc)
{
    this->_locations.push_back(loc);
}

locationType Location::getType()
{
    return this->_locType;
}

std::string Location::getPath() const
{
    // if (this->hasPath() == false)
    //     throw DataNotFoundException();
    return (this->_path);
}

int Location::getClientMaxBodySize() const
{
    // if (this->hasClientMaxBodysize() == false)
    //     throw DataNotFoundException();
    return (this->_clientMaxBodySize);
}

std::string Location::getRoot() const
{
    // if (this->hasRoot() == false)
    //     throw DataNotFoundException();
    return (this->_root);
}

std::string    Location::getIndex() const
{
    // if (this->hasIndex() == false)
    //     throw DataNotFoundException();
    return (this->_index);
}

std::vector<std::string>    Location::getLimitExcept() const
{
    // if (this->hasLimitExcept() == false)
    //     throw DataNotFoundException();
    return (this->_limitExcept);
}

std::map<int, std::string>    Location::getErrorPage() const
{
    // if (this->hasErrorPage() == false)
    //     throw DataNotFoundException();
    return (this->_errorPage);
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

void                        Location::addReturn(int key, std::string val)
{
    (void) key;
    (void) val;
}
std::map<int, std::string>  Location::getReturn() const
{
    return std::map<int, std::string>();
}
bool                        Location::hasReturn() const
{
    return false;
}

// LocationDownload
void                                Location::setAutoIndex(bool ai)
{
    (void) ai;
}
void                                Location::addAddHeader(std::string name, std::string value)
{
    (void) name;
    (void) value;
}
 bool                                Location::getAutoIndex() const
{return false;}
std::map<std::string, std::string>  Location::getAddHeader() const
{
    return std::map<std::string, std::string>();
}
 bool                                Location::hasAddHeader() const
{return false;}

// LocationUpload
void        Location::setUploadStore(std::string uploadStore)
{
    (void) uploadStore;
}
void        Location::setClientBodyTempPath(std::string cbtp)
{
    (void) cbtp;
}
void        Location::setClientBodyFileOnly(bool cbfo)
{
    (void) cbfo;
}
 std::string Location::getUploadStore() const
{
    return "";
}
std::string Location::setClientBodyTempPath() const
{
    return "";
}
bool        Location::getClientBodyFileOnly() const
{return false;}
bool        Location::hasUploadStore() const
{return false;}

// LocationCGI
void                                Location::setPass(std::string pass)
{
    (void) pass;
}
void                                Location::setcgiIndex(std::string idx)
{
    (void) idx;
}
void                                Location::addParam(std::string name, std::string value)
{
    (void) name;
    (void) value;
}
std::string                         Location::getPass() const
{
    return "";
}
std::string                         Location::getCgiIndx() const
{
    return "";
}
std::map<std::string, std::string>  Location::getParam() const
{
    return std::map<std::string, std::string>();
}
bool                                Location::hasPass() const
{
    return false;
}
bool                                Location::hasParam() const
{
    return false;
}

const char* Location::DataNotFoundException::what() const throw()
{
    return ("Data does not exist.");
}

