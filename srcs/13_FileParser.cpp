#include "webserv.hpp"

FileParser::FileParser()
{

}

FileParser::FileParser(char *fileName)
{
	std::ifstream   ifs;

	this->_fileName = fileName;
	this->_currentPos = 0;
	Parse();
}

FileParser::FileParser(const FileParser& rhs)
{
	*this = rhs;
}

FileParser& FileParser::operator=(const FileParser &rhs)
{
	if (this != &rhs)
	{
		this->_fileName = rhs.getFileName();
	}
	return (*this);
}

FileParser::~FileParser()
{
	if (_ifs.is_open())
		_ifs.close();
}

void    FileParser::setFileName(const std::string fileName)
{
	this->_fileName = fileName;
}

void    FileParser::setPosition(std::ifstream& ifs)
{
	if (!ifs.is_open())
		throw FileNotOpenedException();
	this->_currentPos = ifs.tellg();
}

std::string     FileParser::getFileName() const
{
	return (this->_fileName);
}

std::streampos  FileParser::getPosition() const
{
	return (this->_currentPos);
}

std::string FileParser::trimBuf(std::string &buf) const
{
	size_t      annotPos;
	size_t      start;
	size_t      end;
	std::string output;
	
	annotPos = buf.find("#");
	if (annotPos != std::string::npos)
		output = buf.substr(0, annotPos);
	else
		output = buf;
	
	start = output.find_first_not_of(" \t");
	if (start == std::string::npos)
		start = 0;

	end = output.find_last_not_of("  \t");
	if (end == std::string::npos)
		end = output.length() - 1;
	
	output = output.substr(start, end - start + 1);
	return (output);
}

void	FileParser::openFile()
{
	if (this->_ifs.is_open())
		this->_ifs.close();
	this->_ifs.open(this->_fileName.c_str());
	if (!this->_ifs.is_open())
		throw FileNotOpenedException();
	if (this->_currentPos != 0)
	{
		std::cout << "line number" << this->_currentPos << std::endl;
		_ifs.seekg(this->_currentPos);
	}
}

void    FileParser::Parse()
{
	std::vector<std::string>	splitted;
	std::ifstream				ifs;
	std::string					buf;

	openFile();
	buf.clear();
	_config = Config();
	_state = DEFAULT;
	while (getline(_ifs, buf))
	{
		buf = trimBuf(buf);
		if (buf.empty())
			continue;
		
		splitted = preprocessToken(buf);
		
		if (splitted.at(0) == "client_max_body_size")
		{
			std::string	cmds = splitted.at(1);
			int			digit;
			int			clientMaxBodySize = 0;
			
			if (cmds[splitted.at(1).length() - 1] != ';')
				throw SyntaxErrorException();
			else
				cmds.erase(cmds.length() - 1);
			cmds = trimBuf(cmds);
			std::cout << cmds << ":"<< cmds[cmds.length() - 1]<< std::endl;
			if (cmds[cmds.length() - 1] == 'M' || \
				cmds[cmds.length() - 1] == 'm')
				digit = 1000000;
			else if (cmds[cmds.length() - 1]== 'K' || \
				cmds[cmds.length() - 1]== 'k')
				digit = 1000;
			else
				throw SyntaxErrorException();
			
			cmds.erase(cmds.length() - 1);

			for (std::string::iterator it = cmds.begin(); it != cmds.end(); ++it)
			{
				if (std::isdigit(*it) == false)
				{
					std::cerr << "not number" << std::endl;
					throw SyntaxErrorException();
				}
			}
			clientMaxBodySize = std::atoi(cmds.c_str());
			clientMaxBodySize *= digit;
			this->_config.setClientMaxBodySize(clientMaxBodySize);
		}
		
		if (splitted.size() == 1 && splitted.at(0) == "server")
			_state = SERVER_KEYWORD;
		
		if (splitted.size() > 1 && \
				splitted.at(0) == "server" && \
				splitted.at(1) == "{")
		{
			this->setPosition(this->_ifs);
			_ifs.close();
			makeServerBlock();
		}

		if (_state == SERVER_KEYWORD)
		{
			std::cout << splitted.at(0) << std::endl;
			if (splitted.size() == 1 && splitted.at(0) == "{")
			{
				this->setPosition(this->_ifs);
				_ifs.close();
				makeServerBlock();
			}
		}

		buf.clear();
	}
}

std::vector<std::string>	FileParser::preprocessToken(std::string& buf)
{
	char						lstChar;
	std::vector<std::string>	splitted; 

	lstChar = buf[buf.length() - 1];
	if (std::string("{};").find(lstChar) == std::string::npos)
		throw SyntaxErrorException();
	
	splitted = ServerManager::split(buf, ' ');
	if (splitted.size() == 1 && (splitted.at(0) == "{" && splitted.at(0) == "}"))
		throw SyntaxErrorException();
	
	return (splitted);
}

void	FileParser::makeServerBlock()
{
	std::vector<std::string>	splitted;
	std::string					buf;
	Server						serv;

	_state = IN_SERVER;
	openFile();
	
	while (getline(this->_ifs, buf))
	{
		buf = trimBuf(buf);
		if (buf.empty())
			continue;
		splitted = preprocessToken(buf);
		
		if (splitted.at(0) == "listen")
		{
			std::vector<std::string>	listen;

			serv.setListen(ServerManager::split(splitted.at(1), ';').at(0));
			listen = ServerManager::split(serv.getListen(), ' ');
			// serv.setHost(listen.at(0));
			// serv.setPort(std::atoi(listen.at(1).c_str()));
		}
		else if (splitted.at(0) == "server_name")
		{
			serv.setName(ServerManager::split(splitted.at(0), ';').at(0));
		}
		else if (splitted.at(0) == "error_page")
		{
			std::map<int, std::string> errPage;
			std::cout << "Adding errpage" << std::endl;

			errPage[std::atoi(splitted.at(1).c_str())] = splitted.at(2);
			serv.addErrorPage(errPage);
		}
		else if (splitted.at(0) == "location")
		{
			continue;
			// if (splitted.size() == 3 && splitted.at(2) == '{')
			// {
			// 	this->setPosition(this->_ifs);
			// 	this->_ifs.close();
			// 	makeLocationBlock(splitted.at(1));
			// }

		}
		else if (splitted.at(0) == "}")
		{
			break ;
		}
		else
		{
			throw SyntaxErrorException();
		}

	}
	std::cout << serv << std::endl;
	_state = DEFAULT;
	this->_config.addServer(serv);
	this->setPosition(this->_ifs);
	this->_ifs.close();
}

void	makeLocationBlock(std::string root)
{
	root = "";
}

const char* FileParser::FileNotOpenedException::what() const throw()
{
	return ("You MotherFucker");
}

const char* FileParser::SyntaxErrorException::what() const throw()
{
	return ("You MotherFucker");
}