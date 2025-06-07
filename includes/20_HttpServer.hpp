#pragma once
#include "webserv.hpp"
#include "Epoll.hpp"
#include "ServerSocket.hpp"

class HttpServer
{
    private:
		Epoll 						_epoll;
		epoll_event					_events[MAX_EVENTS];
        Config                      _config;
        std::map<int, int>          _serverSockets;
        std::vector<ServerSocket*>   _serverSocketObjs;
        std::map<int, ClientData>   _clients;

        void    setupServerSockets();
        bool    isServerSocket(int fd);

        void    acceptNewConnection(int serverFd);
        int     getServerPort(int serverFd);
        bool    setupClientSocket(int client);
        void    logNewConnection(const struct sockaddr_in& clientAddr, int clientFd);
		void	handleEvent(const epoll_event& event);
        int     handleClientRead(int currentFd);
        int     handleClientWrite(int currentFd);
        
        void    processRequest(ClientData& client);
        void    buildResponse(ClientData& client);
        int     sendResponse(int clinetFd);
        void    closeClientConnection(int clientFd);

        //  요청 처리
        // void        handleGetRequest(ClientData& client);
        // void        handlePostRequest(ClientData& client);
        // void        handleDeleteRequest(ClientData& client);
        // void        handleCgiRequest(ClientData& client, LocationCGI* cgiLocation);

        //  utils
        std::string getMethodString(HttpMethod method);
        std::string buildFilePath(const Location* location, const std::string& path);
        bool        isMethodAllowd(const Location* location, HttpMethod method);
    
    public:
        HttpServer(const Config& config);
        ~HttpServer();
        
        ClientData& getClientData(int currentFd);
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
		class	SocketCreationError: public std::exception
		{
			public:
				const char *what(void) const throw();
		};
		class	SocketConfigError: public std::exception
		{
			public:
				const char *what(void) const throw();
		};
        static std::string extractPath(const std::string& url);
};
