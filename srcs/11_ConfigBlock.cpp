#include "webserv.hpp"

ConfigBlock::ConfigBlock()
{
    this->_directives = std::vector<ConfigDirective>();
    this->_blocks = std::vector<ConfigBlock>();
}

ConfigBlock::ConfigBlock(const std::string& name, const int lvl)
    :_blockLvl(lvl), 
     _name(name)
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

void    ConfigBlock::setBlockLvl(const int lvl)
{
    this->_blockLvl = lvl;
}

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

int ConfigBlock::getBlockLvl() const
{
    return (this->_blockLvl);
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
        ; it != this->_directives.end(); it++)
   {
        if (it->getKey() == dirKey)
            return (*it);
   }
   throw OutOfBoundaryException();
}

const ConfigBlock&  ConfigBlock::searchBlock(const std::string blkName) const
{

    for (std::vector<ConfigBlock>::const_iterator it = this->_blocks.begin() \
        ; it != this->_blocks.end(); it++)
    {
        if (it->getName() == blkName)
            return (*it);
    }
    throw OutOfBoundaryException();
}

int ConfigBlock::directiveLen() const
{
    return (this->_directives.size());
}

int ConfigBlock::blockLen() const
{
    return (this->_blocks.size());
}

std::vector<std::string>    ConfigBlock::getDirectiveList() const
{
    std::vector<std::string>                        vec;
    std::vector<ConfigDirective>::const_iterator    it;

    vec = std::vector<std::string> ();
    it = this->_directives.begin();

    while (it != this->_directives.end())
    {
        vec.push_back(it->getKey());
        it++;
    }
    return (vec);
}

std::vector<std::string>    ConfigBlock::getBlockList() const
{
    std::vector<std::string>                    vec;
    std::vector<ConfigBlock>::const_iterator    it;

    vec = std::vector<std::string> ();
    it  = this->_blocks.begin();

    while (it != this->_blocks.end())
    {
        vec.push_back(it->getName());
        it++;
    }
    return (vec);
}


const char* ConfigBlock::OutOfBoundaryException::what() const throw()
{
    return ("ConfigBlock: Search Failed\n");
}


std::ostream& operator<< (std::ostream& os, const ConfigBlock& configBlk)
{
    os << "ConfigBlock[" << configBlk.getName() << "]: Directives: " << configBlk.directiveLen();
    os << " Blocks: " << configBlk.blockLen() << std::endl;
    return (os);
}