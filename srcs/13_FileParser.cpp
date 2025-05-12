#include "webserv.hpp"

FileParser::FileParser()
{

}

FileParser::FileParser(char *fileName)
{
    std::ifstream   ifs;

    this->_fileName = fileName;
    ifs.open(this->_fileName.c_str());
    setIfstream(ifs);
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

void    FileParser::setIfstream(std::ifstream ifs)
{
    if (!ifs.is_open())
    {
        throw FileNotOpenedException();
    }
    this->_ifs;
}

void    FileParser::setPosition(std::streampos pos)
{
    this->_currentPos = pos;
}

std::string FileParser::getFileName() const
{
    return (this->_fileName);
}

std::string FileParser::trimBuf(std::string &buf) const
{
    size_t      annotPos;
    size_t      start;
    size_t      end;
    std::string output;
    
    annotPos = buf.find("#");
    if (annotPos != std::string::npos)
        output = buf.substr(0, annotPos);
    else
        output = buf;
    
    start = output.find_first_not_of(" \t");
    if (start == std::string::npos)
        start = 0;
    end = output.find_last_not_of("#");

    output = output.substr(start, end - start - 1);
    return (output);
}

void    FileParser::Parse()  const
{
    std::string     buf;
    
    if (ifs.is_open() == false)
        throw FileNotOpenedException();
    
    buf.clear();
    while (getline(ifs, buf))
    {
        std::cout << trimBuf(buf) << std::endl;
        buf.clear();
    }
}

const char* FileParser::FileNotOpenedException::what() const throw()
{
    return ("You MotherFucker");
}