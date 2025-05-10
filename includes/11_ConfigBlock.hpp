#pragma once
#include "webserv.hpp"

class ConfigDirective;

class ConfigBlock
{
    private:
        int                             _blockLvl;
        int                             _valSize;
        int                             _blockSize;
        std::string                     _key;
        std::vector<std::string>        _vals;
        std::vector<ConfigBlock>        _blocks;
        
    public:
        ConfigBlock();
        ConfigBlock(const std::string& name, const int lvl);
        ConfigBlock(const ConfigBlock& rhs);
        ConfigBlock& operator=(const ConfigBlock& rhs);
        ~ConfigBlock();

        void    setBlockLvl(const int lvl);
        void    setName(const std::string& name);
        void    addDirective(const ConfigDirective& dir);
        void    addBlock(const ConfigBlock& block);

        int                            getBlockLvl() const;
        std::string                    getName() const;
        std::vector<ConfigDirective>   getDirectives() const;
        std::vector<ConfigBlock>       getBlocks() const;

        const ConfigDirective&      searchDirective(const std::string& dirKey) const;
        const ConfigBlock&          searchBlock(const std::string blkName) const;
        
        std::vector<std::string>    getDirectiveList() const;
        std::vector<std::string>    getBlockList() const;

        int directiveLen() const;
        int blockLen() const;

        class OutOfBoundaryException: public std::exception
        {
			public:
				const char * what(void) const throw();
		};
};

std::ostream& operator<< (std::ostream& os, const ConfigBlock& configBlk);
