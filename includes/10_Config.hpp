#pragma once
#include "webserv.hpp"

class ConfigBlock;

enum ConfigState
{
    INIT,
    DIRECTIVE_NAME,
    DIRECTIVE_VALUE,
    BLOCK_START,
    BLOCK_CONTENT,
    BLOCK_END,
    COMMENT,
};

class Config
{
    private:
        std::string     _fileName;
        ConfigBlock     _rootBlock;

        void                        parseFile();
    public:
        Config();
        Config(char *fileName);
        Config(const Config &rhs);
        Config& operator=(const Config &rhs);
        ~Config();

        std::string getName() const;
        ConfigBlock getRootBlock() const;

};