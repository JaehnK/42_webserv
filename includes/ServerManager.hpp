#pragma once

class ServerManager
{
    private:
        Config  conf;
        
    public:
        ServerManager();
        ServerManager(const ServerManager &rhs);
        ServerManager &operator =(const ServerManager &rhs);
        ~ServerManager();

};

