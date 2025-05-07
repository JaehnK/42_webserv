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
        ConfigBlock    rootBlock;

    public:
        Config();
        Config(Config &rhs);
        Config&         operator=(Config &rhs);
        ~Config();
};