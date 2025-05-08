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
    parseFile(fileName);
}

Config::Config(const Config &rhs)
{
    this = &rhs;
}

Config& Config::operator=(Config &rhs)
{
    if (*this != rhs)
    {
        this->_fileName = rhs.getName();
        this->_rootBlock = rhs.getRootBlock();
    }
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

void    Parsefile()
{
    
}