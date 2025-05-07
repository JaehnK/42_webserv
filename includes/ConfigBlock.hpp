#pragma once
#include "webserv.hpp"

class ConfigBlock
{
    private:
        std::string     nodeName;
        ConfigBlock*     childNode;

    public:
        ConfigBlock( );
        ~ConfigBlock( );
};

