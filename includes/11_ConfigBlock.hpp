#pragma once
#include "webserv.hpp"

class ConfigDirective;

class ConfigBlock
{
    private:
        std::string                     _name;
        std::vector<ConfigDirective>    _directives;
        std::vector<ConfigBlock>        _blocks;
        
        ConfigBlock();
    public:
        ConfigBlock(const std::string& name);
        ConfigBlock(const ConfigBlock& rhs);
        ConfigBlock& operator=(const ConfigBlock& rhs);
        ~ConfigBlock();

        void    setName(const std::string& name);
        void    addDirective(const ConfigDirective& dir);
        void    addBlock(const ConfigBlock& block);

        std::string                    getName() const;
        std::vector<ConfigDirective>   getDirectives() const;
        std::vector<ConfigBlock>       getBlocks() const;

        const ConfigDirective&       searchDirective(const std::string& dirKey) const;
        const ConfigBlock&          searchBlock(const std::string blkName) const;
        
        std::vector<std::string>    getDirectiveList() const;
        std::vector<std::string>    getBlockList() const;

        int directiveLen();
        int blockLen();

        class OutOfBoundaryException: public std::exception
        {
			public:
				const char * what(void) const throw();
		};
};

