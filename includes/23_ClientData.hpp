#pragma once
#include "webserv.hpp"

class ClientData
{
    private:
        int             _socketFd;
        int             _serverPort;
        bool            _reqReady;
        bool            _respReady;
        const Server*   _server;
        const Location* _location;
        HttpRequest*    _req;
        HttpResponse*   _resp;

        ClientData();

    public:
        ClientData(int clientFd);
        ClientData(const ClientData& rhs);
        ClientData& operator=(const ClientData& rhs);
        ~ClientData();

        void    setServerPort(int port);
        void    setReqReady(bool ready);
        void    setRespReady(bool ready);
        void    setServer(Config& conf);
        void    setLocation();
        void    setReq(HttpRequest* req);
        void    setResp(HttpResponse* resp);

        int             getSocketFd() const;
        bool            getReqReady() const;
        bool            getRespReady() const;
        const Server*   getServer() const;
        const Location* getLocation() const;
        HttpRequest*    getReq() const;
        HttpResponse*   getResp() const;

        void    createResponse();

        class   InvalidConfigException: public std::exception
        {
            public:
                const char *what(void) const throw();

        };
};
