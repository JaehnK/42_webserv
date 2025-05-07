#include "webserv.hpp"

int main(int argc, char **argv)
{
    ServerManager serverManager;

    serverManager = ServerManager();
    if (argc > 1)
        serverManager.setConfig(argv[1]);
    else
        serverManager.defaultConfig();
    
    return (0);
}