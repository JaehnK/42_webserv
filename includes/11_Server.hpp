#include "webserv.hpp"

class Server:
{
    private:
        std::string                                 _name;
        std::string                                 _host;
        int                                         _port;
        std::string                                 _listen;
        std::string                                 _root;
        std::string                                 _serverName;
        std::vector<std::map<int, std::string> >    _errorPage;
        std::vector<Location>                       _location;
        
    public:
        Server();
        Server(const Server &rhs);
        Server& operator=(const Server &rhs);
        ~Server();

        void    setName(std::string name);
        void    setHost(std::string host);
        void    setPort(int port);
        void    setListen(std::string listen);
        void    setRoot(std::string root);
        void    setServerName
        
};

std::ostream	&operator<<(std::ostream& os, const Server& serv);

