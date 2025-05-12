#pragma once
#include "webserv.hpp"

class Location
{
    protected:
        std::string                                 _path;
        int                                         _clientMaxBodySize;
        std::string                                 _root;
        std::vector<std::string>                    _index;
        std::vector<std::string>                    _limitExcept;
        std::vector<std::map<int, std::string> >    _errorPage;
        bool                                        _denyAll;
    
    public:
        Location();
        Location(const Location& rhs);
        Location&   operator=(const Location& rhs);
        ~Location();

        // Setters
        void    setPath(std::string path);
        void    setClientMaxBodySize(int cmbs);
        void    setRoot(std::string root);
        void    setDenyall(bool denyall);
        void    addIndex(std::string idx);
        void    addLimitExcept(std::string limitExcept);
        void    addErrorPage(std::map<int, std::string> errPage);

        // Getters
        std::string                                 getPath() const;
        int                                         getClientMaxBodySize() const;
        std::string                                 getRoot() const;
        std::vector<std::string>                    getIndex() const;
        std::vector<std::string>                    getLimitExcept() const;
        std::vector<std::map<int, std::string> >    getErrorPage() const;
        bool                                        getDenyAll() const;

        // Checkers
        bool    hasPath() const;
        bool    hasClientMaxBodysize() const;
        bool    hasRoot() const;
        bool    hasIndex() const;
        bool    hasLimitExcept() const;
        bool    hasErrorPage() const;

        class DataNotFoundException: public std::exception
        {
            public:
                virtual const char* what() const throw();
                virtual ~DataNotFoundException() throw(){};
        };

};