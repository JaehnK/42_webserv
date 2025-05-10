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
        Server(/* args */);
        ~Server();
};
