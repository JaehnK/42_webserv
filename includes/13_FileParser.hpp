#pragma once
#include "webserv.hpp"

class FileParser
{
private:
    std::string _fileName;
    FileParser();

public:
    FileParser(char *fileName);
    FileParser(const FileParser& rhs);
    FileParser& operator=(const FileParser &rhs);
    ~FileParser();

    void        setFileName(const std::string fileName);
    std::string getFileName() const;

    void    Parse() const;
    
    class FileNotOpenedException: public std::exception
    {
        public:
            const char* what() const throw();
    };
};
