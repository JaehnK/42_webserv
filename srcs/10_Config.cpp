#include "webserv.hpp"

Config::Config()
{
    this->_rootBlock = ConfigBlock("root");
    this->_fileName = "default";
}

Config::Config(char *fileName)
{
    this->_rootBlock = ConfigBlock("root");
    this->_fileName = fileName;
    parseFile();
}

Config::Config(const Config &rhs)
{
    *this = rhs;
}

Config& Config::operator=(const Config &rhs)
{
    if (this != &rhs)
    {
        this->_fileName = rhs.getName();
        this->_rootBlock = rhs.getRootBlock();
    }
    return (*this);
}

Config::~Config()
{}


std::string Config::getName() const
{
    return (this->_fileName);
}

ConfigBlock Config::getRootBlock() const
{
    return (this->_rootBlock);
}

void    Config::parseFile()
{
    std::ifstream ifs;
    
    ifs.open(this->_fileName.c_str());

}