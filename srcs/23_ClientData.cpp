#include "webserv.hpp"

const char* ClientData::InvalidConfigException::what() const throw()
{
    return "Invalid configuration";
}

ClientData::ClientData()
{}

ClientData::ClientData(int clientFd)
    :_socketFd(clientFd)
{
    this->_reqReady = false;
    this->_respReady = false;
    this->_server = NULL;
    this->_location = NULL;
    this->_req = NULL;
    this->_resp = NULL;
}

ClientData::ClientData(const ClientData& rhs)
{
	*this = rhs;
}

ClientData& ClientData::operator=(const ClientData& rhs)
{
    if (this != &rhs)
    {
        this->_socketFd = rhs.getSocketFd();
        this->_reqReady = rhs.getReqReady();
        this->_respReady = rhs.getRespReady();
        this->_server = rhs.getServer();
        this->_location = rhs.getLocation();
        this->_req = rhs.getReq();
        this->_resp = rhs.getResp();
    }
    return (*this);
}

ClientData::~ClientData()
{

}

void    ClientData::setServerPort(int port)
{
    this->_serverPort = port;
}

void    ClientData::setReqReady(bool ready)
{
    this->_reqReady = ready;
}

void    ClientData::setRespReady(bool ready)
{
    this->_respReady = ready;
}

void    ClientData::setServer(Config& conf)
{
    const std::vector<Server>& servers = conf.getServers();

    if (servers.empty())
        throw InvalidConfigException();

    for (std::vector<Server>::const_iterator it = servers.begin(); 
            it != servers.end(); ++it) 
        {
            if (it->getPort() == this->_serverPort)
            {
                _server = &(*it);
                return;
            }
        }
   _server = &servers[0];
}

void    ClientData::setLocation()
{
    const std::vector<Location*>&          locations = this->_server->getLocations();
    std::string                     requestUrl = this->_req->getUrl();
    std::string                     locationPath;
    Location*                       bestMatch = NULL;
    size_t                          longestMatch = 0;

    for (std::vector<Location*>::const_iterator it = locations.begin(); \
        it != locations.end(); ++it)
    {
        locationPath = (*it)->getPath();
        
        if (requestUrl.find(locationPath) == 0)
        {
            if (locationPath.length() > longestMatch)
            {
                longestMatch = locationPath.length();
                bestMatch = *it;
            }
        }
    }

    if (bestMatch != NULL)
        this->_location = bestMatch;
    else if (!locations.empty())
        this->_location = locations[0];   // 기본값으로 첫 번째 location
    else
        throw  InvalidConfigException();
}

void    ClientData::setReq(HttpRequest* req)
{
    this->_req = req;
}

void    ClientData::setResp(HttpResponse* resp)
{
    this->_resp = resp;
}

int             ClientData::getSocketFd() const
{
    return (this->_socketFd);
}

bool            ClientData::getReqReady() const
{
    return (this->_reqReady);
}

bool            ClientData::getRespReady() const
{
    return (this->_respReady);
}

const Server*   ClientData::getServer() const
{
    return (this->_server);
}

const Location* ClientData::getLocation() const
{
    return (this->_location);
}

HttpRequest*    ClientData::getReq() const
{
    return (this->_req);
}

HttpResponse*   ClientData::getResp() const
{
    return (this->_resp);
}

void    ClientData::createResponse()
{
    this->_resp->checkAllowedMethod(this->_location->getLimitExcept());
    this->_resp->handleMethod();
    this->_resp->assembleMsg();
    this->_respReady = true;
}