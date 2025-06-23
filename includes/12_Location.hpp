#pragma once
#include "webserv.hpp"

enum    locationType
{
    DEFAULT,
    API,
    DOWNLOAD,
    UPLOAD,
    CGI,
};

class Location
{
    protected:
        locationType                                _locType;
        std::string                                 _path;
        int                                         _clientMaxBodySize;
        std::string                                 _root;
        std::string                                 _index;
        std::vector<std::string>                    _limitExcept;
        std::map<int, std::string>                  _errorPage;
        std::vector<Location *>                     _locations;

     
    
    public:
        Location();
        Location(const Location& rhs);
        Location&   operator=(const Location& rhs);
        virtual ~Location();

        virtual Location* clone() const = 0;
        
        // Setters
        void    setPath(std::string path);
        void    setClientMaxBodySize(int cmbs);
        void    setRoot(std::string root);
        // void    setDenyall(bool denyall);
        void    setIndex(std::string idx);
        void    addLimitExcept(std::string limitExcept);
        void    addErrorPage(int key, std::string value);
        void    addLocations(Location* loc);

        // Getters
        std::vector<Location *>                     getLocations() const;
        locationType                                getType() const;
        std::string                                 getPath() const;
        std::string                                 getPath();
        int                                         getClientMaxBodySize() const;
        std::string                                 getRoot() const;
        std::string                                 getIndex() const;
        std::vector<std::string>                    getLimitExcept() const;
        std::map<int, std::string>                  getErrorPage() const;

        // Checkers
        bool    hasPath() const;
        bool    hasClientMaxBodysize() const;
        bool    hasRoot() const;
        bool    hasIndex() const;
        bool    hasLimitExcept() const;
        bool    hasErrorPage() const;

        // LocationAPI
        virtual void                        addReturn(int key, std::string val);
        virtual std::map<int, std::string>  getReturn() const;
        virtual bool                        hasReturn() const;

        // LocationDownload
        virtual void                                setAutoIndex(bool ai);
        virtual void                                addAddHeader(std::string name, std::string value);
        virtual bool                                getAutoIndex() const;
        virtual std::map<std::string, std::string>  getAddHeader() const;
        virtual bool                                hasAddHeader() const;

        // LocationUpload
        virtual void        setUploadStore(std::string uploadStore);
        virtual void        setClientBodyTempPath(std::string cbtp);
        virtual void        setClientBodyFileOnly(bool cbfo);
        virtual std::string getUploadStore() const;
        virtual std::string setClientBodyTempPath() const;
        virtual bool        getClientBodyFileOnly() const;
        virtual bool        hasUploadStore() const;

        // LocationCGI
        virtual void                                setPass(std::string pass);
        virtual void                                setcgiIndex(std::string idx);
        virtual void                                addParam(std::string name, std::string value);
        virtual std::string                         getPass() const;
        virtual std::string                         getCgiIndx() const;
        virtual std::map<std::string, std::string>  getParam() const;

        virtual bool                                hasPass() const;
        virtual bool                                hasParam() const;
        
        class DataNotFoundException: public std::exception
        {
            public:
                virtual const char* what() const throw();
                virtual ~DataNotFoundException() throw(){};
        };

};