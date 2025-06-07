#include "webserv.hpp"

const char* ClientData::InvalidConfigException::what() const throw()
{
    return "Invalid configuration";
}

ClientData::ClientData() : _server(NULL), _location(NULL), _req(NULL), _resp(NULL)
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
        this->_serverPort = rhs.getServerPort();
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
    if (_req != NULL)
    {
        delete _req;
        _req = NULL;
    }
    if (_resp != NULL)
    {
        delete _resp;
        _resp = NULL;
    }
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
    const std::vector<Server> &servers = conf.getServers();

    if (servers.empty())
        throw InvalidConfigException();

    for (size_t i = 0; i < servers.size(); ++i)
    {
        if (servers[i].getPort() == this->_serverPort)
        {
            _server = &servers[i];
            return;
        }
    }
    // for (std::vector<Server>::const_iterator it = servers.begin(); 
    //         it != servers.end(); ++it) 
    //     {
    //         if (it->getPort() == this->_serverPort)
    //         {
    //             _server = &(*it);
    //             return;
    //         }
    //     }
   _server = &servers[0];
}

// void    ClientData::setLocation()
// {
//     const std::vector<Location*>&          locations = this->_server->getLocations();
//     std::string                     requestUrl = this->_req->getUrl();
//     std::string                     locationPath;
//     Location*                       bestMatch = NULL;
//     size_t                          longestMatch = 0;

//     for (std::vector<Location*>::const_iterator it = locations.begin(); \
//         it != locations.end(); ++it)
//     {
//         std::cout << (*it)->getPath() << std::endl;
//         locationPath = (*it)->getPath();
        
//         if (requestUrl.find(locationPath) == 0)
//         {
//             if (locationPath.length() > longestMatch)
//             {
//                 longestMatch = locationPath.length();
//                 bestMatch = *it;
//             }
//         }
//     }

//     if (bestMatch != NULL)
//         this->_location = bestMatch;
//     else if (!locations.empty())
//         this->_location = locations[0];   // 기본값으로 첫 번째 location
//     else
//         throw  InvalidConfigException();
// }


void ClientData::setLocation()
{
    std::cout << "=== setLocation() started ===" << std::endl;
    
    // 서버가 null인지 먼저 확인
    if (this->_server == NULL)
    {
        std::cerr << "Error: Server is NULL!" << std::endl;
        throw InvalidConfigException();
    }
    
    std::cout << "Server found, getting locations..." << std::endl;
    
    const std::vector<Location*>& locations = this->_server->getLocations();
    std::cout << "Locations count: " << locations.size() << std::endl;
    
    // 빈 벡터 체크
    if (locations.empty())
    {
        std::cout << "No locations configured, creating default location" << std::endl;
        // 기본 location을 NULL로 설정하고 계속 진행
        this->_location = NULL;
        return;
    }
    
    std::string requestUrl = this->_req->getUrl();
    std::cout << "Request URL: " << requestUrl << std::endl;
    
    Location* bestMatch = NULL;
    size_t longestMatch = 0;
    
    // 각 location을 안전하게 체크
    for (size_t i = 0; i < locations.size(); ++i)
    {
        std::cout << "Checking location " << i << std::endl;
        
        if (locations[i] == NULL)
        {
            std::cout << "Warning: Location[" << i << "] is NULL!" << std::endl;
            continue;
        }
        try
        {
            std::cout << i << std::endl;
            std::string locationPath = locations[i]->getPath();
            std::cout << "Location path: [" << locationPath << "]" << std::endl;
            
            if (requestUrl.find(locationPath) == 0)
            {
                if (locationPath.length() > longestMatch)
                {
                    longestMatch = locationPath.length();
                    bestMatch = locations[i];
                    std::cout << "New best match: " << locationPath << std::endl;
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Error getting path from location[" << i << "]: " << e.what() << std::endl;
            continue;
        }
        std::cout <<"\n";
    }

    if (bestMatch != NULL)
    {
        std::cout << "Best match found!" << std::endl;
        this->_location = bestMatch;
    }
    else
    {
        std::cout << "No match found, using first location" << std::endl;
        // 첫 번째 location이 유효한지 확인
        if (locations[0] != NULL)
        {
            try
            {
                std::string testPath = locations[0]->getPath();
                this->_location = locations[0];
                std::cout << "Using default location: " << testPath << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cout << "Error with default location: " << e.what() << std::endl;
                this->_location = NULL;
            }
        }
        else
        {
            std::cout << "Default location is NULL!" << std::endl;
            this->_location = NULL;
        }
    }
}

void    ClientData::setReq(HttpRequest* req)
{
    this->_req = req;
}

void    ClientData::setResp(HttpResponse* resp)
{
    this->_resp = resp;
}

int             ClientData::getServerPort() const
{
    return (this->_serverPort);
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

// void ClientData::createResponse()
// {
//     std::cout << "=== Creating response ===" << std::endl;
//     std::cout << "Request URL: " << this->_req->getUrl() << std::endl;
//     std::cout << "Request Method: " << this->_req->getMethod() << std::endl;
    
//     std::vector<std::string> limit;
//     if (this->_location != NULL)
//     {
//         std::cout << "Location found: " << this->_location->getPath() << std::endl;
//         limit = this->_location->getLimitExcept();
//     }
//     else
//     {
//         std::cout << "No location found, using default" << std::endl;
//     }
    
//     if (!this->_resp->checkAllowedMethod(limit))
//     {
//         std::cout << "Method not allowed" << std::endl;
//         this->_resp->assembleMsg();
//         this->_respReady = true;
//         return;
//     }

//     this->_resp->handleMethod();
//     std::cout << "Response status code: " << this->_resp->getStatCode() << std::endl;
    
//     this->_resp->assembleMsg();
//     this->_respReady = true;
    
//     std::cout << "Response ready!" << std::endl;
// }

void ClientData::createResponse()
{
    std::cout << "=== Creating response ===" << std::endl;
    
    std::vector<std::string> limit;
    
    // Location이 NULL이어도 계속 진행
    if (this->_location != NULL)
    {
        try
        {
            limit = this->_location->getLimitExcept();
            std::cout << "Using location limits" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Error getting limits: " << e.what() << std::endl;
            // 빈 벡터로 계속 진행
        }
    }
    else
    {
        std::cout << "No location set, allowing all methods" << std::endl;
        // 빈 벡터면 모든 메소드 허용
    }
    
    if (!this->_resp->checkAllowedMethod(limit))
    {
        std::cout << "Method not allowed" << std::endl;
        this->_resp->assembleMsg();
        this->_respReady = true;
        return;
    }

    std::cout << "Handling method..." << std::endl;
    this->_resp->handleMethod();
    
    std::cout << "Assembling message..." << std::endl;
    this->_resp->assembleMsg();
    
    this->_respReady = true;
    std::cout << "Response created successfully!" << std::endl;
}