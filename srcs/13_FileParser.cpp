#include "webserv.hpp"

FileParser::FileParser()
{

}

FileParser::FileParser(char *fileName)
{
    this->_fileName = fileName;
    Parse();
}

FileParser::FileParser(const FileParser& rhs)
{
    *this = rhs;
}

FileParser& FileParser::operator=(const FileParser &rhs)
{
    if (this != &rhs)
    {
        this->_fileName = rhs.getFileName();
    }
    return (*this);
}

FileParser::~FileParser()
{

}


void    FileParser::setFileName(const std::string fileName)
{
    this->_fileName = fileName;
}

std::string FileParser::getFileName() const
{
    return (this->_fileName);
}

void    FileParser::Parse()  const
{
    std::ifstream   ifs;
    std::string     buf;

    ifs.open(this->_fileName.c_str());
    
    if (ifs.is_open() == false)
        throw FileNotOpenedException();
    
    buf.clear();
    while (getline(ifs, buf))
    {
        buf.find_last_not_of('#');
        buf.find_first_not_of('\t');
        std::cout << buf << std::endl;
    }
}

const char* FileParser::FileNotOpenedException::what() const throw()
{
    return ("You MotherFucker");
}