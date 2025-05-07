#pragma once
#include "webserv.hpp"

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
        /* data */
    public:
        Config();
        ~Config();
};