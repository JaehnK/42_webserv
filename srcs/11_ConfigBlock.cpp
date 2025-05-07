#include "webserv.hpp"

ConfigBlock::ConfigBlock()
{
    this->_directives = std::vector<ConfigDirective>();
    this->_blocks = std::vector<ConfigBlock>();
}

ConfigBlock::ConfigBlock(const std::string& name): _name(name)
{
    this->_directives = std::vector<ConfigDirective>();
    this->_blocks = std::vector<ConfigBlock>();
}

ConfigBlock::ConfigBlock(const ConfigBlock& rhs)
{
    *this = rhs;
}

ConfigBlock& ConfigBlock::operator=(const ConfigBlock& rhs)
{
    if (this != &rhs)
    {
        this->_name = rhs.getName();
        this->_directives = rhs.getDirectives();
        this->_blocks = rhs.getBlocks();
    }
    return (*this);
}

ConfigBlock::~ConfigBlock()
{}

void    ConfigBlock::setName(const std::string& name)
{
    this->_name = name;
}

void    ConfigBlock::addDirective(const ConfigDirective& dir)
{
    this->_directives.push_back(dir);
}

void    ConfigBlock::addBlock(const ConfigBlock &blk)
{
    this->_blocks.push_back(blk);
}

std::string ConfigBlock::getName() const
{
    return (this->_name);
}

std::vector<ConfigDirective> ConfigBlock::getDirectives() const
{
    return (this->_directives);
}

std::vector<ConfigBlock>    ConfigBlock::getBlocks() const
{
    return (this->_blocks);
}

const ConfigDirective&   ConfigBlock::searchDirective(const std::string& dirKey) const
{
   for (std::vector<ConfigDirective>::const_iterator it = this->_directives.begin() \
        ; it != this->_directives.end(); i++)
   {
        if (it->getKey() == dirKey)
            return (*it);
   }
   throw OutOfBoundaryException();
}

const ConfigBlock&  ConfigBlock::searchBlock(const std::string blkName) const
{

    for (std::vector<ConfigBlock>::const_iterator it = this->_blocks.begin() \
        ; it != this->_blocks.end(); i++)
    {
        if (it->getName() == blkName)
            return (*it);
    }
    throw OutOfBoundaryException();
}

int ConfigBlock::directiveLen()
{
    return (this->_directives.size());
}

int ConfigBlock::blockLen()
{
    return (this->_blocks.size());
}

const char* ConfigBlock::OutOfBoundaryException::what() const throw()
{
    return ("ConfigBlock: Search Failed\n");
}
