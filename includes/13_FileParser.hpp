#pragma once
#include "webserv.hpp"

class FileParser
{
    private:
        std::string     _fileName;
        std::ifstream   _ifs;
        std::streampos  _currentPos;
        
        FileParser();
        std::string trimBuf(std::string &buf) const;
        
    public:
        FileParser(char *fileName);
        FileParser(const FileParser& rhs);
        FileParser& operator=(const FileParser &rhs);
        ~FileParser();

        void        setFileName(const std::string fileName);
        void        setIfstream(const std::string fileName);
        void        setPosition(std::streampos pos);
        std::string     getFileName() const;
        std::ifstream&  getIfstream() const;
        std::streampos  getPosition() const;

        void    Parse() const;
        


        class FileNotOpenedException: public std::exception
        {
            public:
                const char* what() const throw();
        };
};
