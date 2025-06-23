#include "webserv.hpp"

int main(int argc, char **argv)
{
    // ServerManager manager;

    // if (argc == 1)
    //     manager.loadConfig();
    // else if (argc == 2)
    //     manager.loadConfig(argv[1]);
    // else
    //     return (-1);
    // manager.initialize();
    // int result = manager.run();
    // return (result);
    if (argc == 2)
    {
        FileParser     fp(argv[1]);
        Config         conf = fp.getConfig();
        std::cout << "Starting server run loop..." << std::endl;
    }
    return (0);
}
