#pragma once
#include "webserv.hpp"

class HttpServer
{
private:
    Config  _config;
    std::map<int, int> _serverSockets;
    fd_set  _readFds;
    fd_set  _writeFd;
    int     _maxFd;

    struct ClientData
    {
        int         socketFd;
        std::string request;
        std::string response;
        bool        requestComplete;
        bool        responseReady;
        Server*     server;
        Location*   location;
    };
    
    std::map<int, ClientData>   _clients;
public:
    HttpServer(const Config& config);
    ~HttpServer();

    void    initialize();   //  소켓 바인딩
    void    run();  //  서버 루프

private:
    int     setupServerSockets();
    void    acceptNewConnection(int serverFd);
    void    handleClientRequest(int clientFd);
    void    processRequset(ClientData& client);
    void    buildResponse(ClientData& client);
    void    sendResponse(int clinetFd);

    //  요청 처리
    Server*     findMatchingServer(const std::string& host, int port);
    Location*   findMatchingLocation(Server* server, const std::string& path);
    void        handleGetRequest(ClientData& client);
    void        handlePostRequest(ClientData& client);
    void        handleDeleteRequest(ClientData& client);
    void        handleCgiRequest(ClientData& client, LocationCGI* cgiLocation);
};
