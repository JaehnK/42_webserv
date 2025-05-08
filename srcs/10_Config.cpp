#include "webserv.hpp"

Config::Config()
{
    this->_rootBlock = ConfigBlock("root", 0);
    this->_fileName = "default";
}

Config::Config(char *fileName)
{
    this->_rootBlock = ConfigBlock("root", 0);
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
    std::ifstream   ifs;
    std::string     buf;
    ConfigBlock     nowBlock;

    nowBlock = this->_rootBlock;
    ifs.open(this->_fileName.c_str());
    while (std::getline(ifs, buf))
    {
        std::cout << buf << std::endl;
        size_t annotPos = buf.find("#");
        if (annotPos != std::string::npos)
            std::cout << "Remove annotation: " << buf.substr(buf.size() - annotPos - 1) << std::endl;

    }
    ifs.close();

}