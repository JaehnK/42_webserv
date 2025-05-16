#include "webserv.hpp"

class Location;

class Server
{
    private:
        std::string                 _name;
        std::string                 _host;
        int                         _port;
        std::string                 _listen;
        std::string                 _root;
        std::map<int, std::string>  _errorPages;
        std::vector<Location*>       _locations;
        
    public:
        Server();
        Server(const Server& rhs);
        Server& operator=(const Server& rhs);
        ~Server();

        // Setters
        void    setName(const std::string& name);
        void    setHost(const std::string& host);
        void    setPort(int port);
        void    setListen(const std::string& listen);
        void    setRoot(const std::string& root);
        void    addErrorPage(int key, std::string value);
        void    addLocation(Location* location);

        // Getters
        std::string                     getName() const;
        std::string                     getHost() const;
        int                             getPort() const;
        std::string                     getListen() const;
        std::string                     getRoot() const;
        std::map<int, std::string>      getErrorPages() const;
        std::vector<Location*>          getLocations() const;

        // Checkers
        bool    hasName() const;
        bool    hasHost() const;
        bool    hasPort() const;
        bool    hasListen() const;
        bool    hasRoot() const;
        bool    hasErrorPages() const;
        bool    hasLocations() const;

        // size
        size_t  errPagesSize() const;
        size_t  locationSize() const;
        
        class DataNotFoundException: public std::exception
        {
            public:
                const char* what() const throw();
        };
};

std::ostream	&operator<<(std::ostream& os, const Server& serv);

