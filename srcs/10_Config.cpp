#include "webserv.hpp"

Config::Config()
{
    this->_fileName = "default";
    this->_clientMaxBodySize = 0;
    this->_servers = std::vector<Server> ();
}

Config::Config(char *fileName)
{
    this->_fileName = fileName;
    this->_clientMaxBodySize = 0;
    this->_servers = std::vector<Server> ();

    parseFile();
}

Config::Config(const Config &rhs)
{
    *this = rhs;
}

Config& Config::operator=(const Config &rhs)
{
    if (this != &rhs)
    {
        this->_fileName = rhs.getFileName();
        this->_clientMaxBodySize = rhs.getClientMaxBodySize();
        this->_servers = rhs.getServers();
    }
    return (*this);
}

Config::~Config()
{}

void    Config::setFileName(std::string fileName)
{
    this->_fileName = fileName;
}

void    Config::setClientMaxBodySize(int clientMaxBodySize)
{
    this->_clientMaxBodySize = clientMaxBodySize;
}

void    Config::addServer(Server server)
{
    this->_servers.push_back(server);
}

std::string Config::getFileName() const
{
    return (this->_fileName);
}

int Config::getClientMaxBodySize() const
{
    return (this->_clientMaxBodySize);
}

std::vector<Server> Config::getServers() const
{
    return (this->_servers);
}

void    Config::parseFile()
{
    std::ifstream   ifs;
    std::string     buf;
    ConfigBlock     nowBlock;

    nowBlock = this->_rootBlock;
    ifs.open(this->_fileName.c_str());
    while (std::getline(ifs, buf))
    {
        // std::cout << buf << std::endl;
        // size_t annotPos = buf.find("#");
        // if (annotPos != std::string::npos)
        //     std::cout << "Remove annotation: " << annotPos << " "<< buf.substr(annotPos + 1) << std::endl;
    
        std::string::iterator it = buf.begin();
        size_t indentPos = 0;
        while (it != buf.end())
        {
            if (*it != '\t' && *it !=' ')
                break ;
            indentPos++;
            it++;
        }
        std::cout << buf.substr(indentPos, buf.size()) << std::endl;
        buf.find_first_not_of()
    }
    ifs.close();

}