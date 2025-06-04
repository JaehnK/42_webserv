#pragma once
#include "webserv.hpp"
#include "Epoll.hpp"

class HttpServer
{
    private:
        int                         _epollFd;
        Config                      _config;
        std::map<int, int>          _serverSockets;
        std::map<int, ClientData>   _clients;

		Epoll 						_epoll;

        int     setupServerSockets();
        void    initaliseEpoll(int *epollFd);
        bool    isServerSocket(int currentFd);
        
        void    acceptNewConnection(int serverFd, int epollFd);
        int     getServerPort(int serverFd);
        bool    setupClientSocket(int client);
        void    logNewConnection(const struct sockaddr_in& clientAddr, int clientFd);
        bool    registerClientToEpoll(int clientFd, int epollFd);

        int     handleClientRead(int currentFd);
        int     handleClientWrite(int currentFd);
        
        void    processRequest(ClientData& client);
        void    buildResponse(ClientData& client);
        int     sendResponse(int clinetFd);
        void    closeClientConnection(int clientFd, int epollFd);

        //  요청 처리
        void        handleGetRequest(ClientData& client);
        void        handlePostRequest(ClientData& client);
        void        handleDeleteRequest(ClientData& client);
        void        handleCgiRequest(ClientData& client, LocationCGI* cgiLocation);

        //  utils
        std::string getMethodString(HttpMethod method);
        std::string buildFilePath(const Location* location, const std::string& path);
        bool        isMethodAllowd(const Location* location, HttpMethod method);
    
    public:
        HttpServer(const Config& config);
        ~HttpServer();
        
        ClientData& getClientData(int currentFd);
        void        initialize();   //  소켓 바인딩
        void        run();  //  서버 루프
        
        class   FailedSocket: public std::exception
        {
            public:
            const char *what(void) const throw();
        };
        
        class   InvalidCurrentFd: public std::exception
        {
            public:
            const char *what(void) const throw();
            
        };
        static std::string extractPath(const std::string& url);
};
