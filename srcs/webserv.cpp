#include "webserv.hpp"

int main(int argc, char **argv)
{

    if (argc == 2)
    {
        FileParser     fp(argv[1]);        
        Config         conf = fp.getConfig();
        HttpServer     server(conf);
    }
    
    return (0);
}