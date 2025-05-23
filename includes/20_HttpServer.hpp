#pragma once
#include "webserv.hpp"

class HttpServer
{
private:
    struct ClientData
    {
        int             socketFd;
        std::string     request;
        std::string     response;
        bool            requestComplete;
        bool            responseReady;
        Server*         server;
        Location*       location;
        HttpRequest*    parseRequest;
    };

    Config  _config;
    std::map<int, int> _serverSockets;
    std::map<int, ClientData>   _clients;
    int     _epollFd;

    int     setupServerSockets();
    void    acceptNewConnection(int serverFd, int epollFd);
    void    processRequest(ClientData& client);
    void    buildResponse(ClientData& client);
    int     sendResponse(int clinetFd);
    void    closeClientConnection(int clientFd, int epollFd);

    //  요청 처리
    Server*     findMatchingServer(const std::string& host, int port);
    Location*   findMatchingLocation(Server* server, const std::string& path);
    void        handleGetRequest(ClientData& client);
    void        handlePostRequest(ClientData& client);
    void        handleDeleteRequest(ClientData& client);
    void        handleCgiRequest(ClientData& client, LocationCGI* cgiLocation);

    //  utils
    std::string getMethodString(HttpMethod method);
    std::string extractPath(const std::string& url);
    std::string buildFilePath(const Location* location, const std::string& path);
    bool        isMethodAllowd(const Location* location, HttpMethod method);
public:
    HttpServer(const Config& config);
    ~HttpServer();

    void    initialize();   //  소켓 바인딩
    void    run();  //  서버 루프

    class   FailedSocket: public std::exception
    {
        public:
            const char *what(void) const throw();
    };
};
