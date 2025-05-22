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
	// if (!ifs.is_open())
	// 	throw FileNotOpenedException();
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

Config			FileParser::getConfig() const
{
	return (this->_config);
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
		std::cout << "File Open number: " << this->_currentPos << std::endl;
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
		std::cout << "Top: " <<buf << std::endl;
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
		{
			_state = SERVER_KEYWORD;
			continue;
		}
		
		if (splitted.size() > 1 && \
				splitted.at(0) == "server" && \
				splitted.at(1) == "{")
		{
			this->setPosition(this->_ifs);
			_ifs.close();
			makeServerBlock();
			openFile();
			continue;
		}

		if (_state == SERVER_KEYWORD)
		{
			if (splitted.size() == 1 && splitted.at(0) == "{")
			{
				this->setPosition(this->_ifs);
				_ifs.close();
				makeServerBlock();
				openFile();
			continue;

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
	if (buf == "server" || buf.find("location") != std::string::npos)
		;
	else if (std::string("{};").find(lstChar) == std::string::npos)
	{
		std::cout << "Test Failed: " << buf << std::endl;
		throw SyntaxErrorException();
	}
	
	splitted = ServerManager::split(buf, ' ');
	if (splitted.size() == 1 && (splitted.at(0) == "{" && splitted.at(0) == "}"))
		throw SyntaxErrorException();
	
	return (splitted);
}

void	FileParser::makeServerBlock()
{
	std::vector<std::string>	splitted;
	std::string 				LocationRoot;
	std::string					buf;
	Server						serv;

	_state = IN_SERVER;
	openFile();
	
	while (getline(this->_ifs, buf))
	{
		if (this->_ifs.tellg() == -1)
			break ;
		buf = trimBuf(buf);
		if (buf.empty())
			continue;
		std::cout << "Serv: " << buf << std::endl;
		splitted = preprocessToken(buf);

		if (_state == LOCATION_KEYWORD)
		{
			if (splitted.size() == 1 && splitted.at(0) == "{")
			{
				this->setPosition(this->_ifs);
				this->_ifs.close();
				serv.addLocation(makeLocationBlock(LocationRoot));
				openFile();
			}
			else
				throw SyntaxErrorException();
		}
		
		if (splitted.at(0) == "listen")
		{
			std::vector<std::string>	listen;

			serv.setListen(ServerManager::split(splitted.at(1), ';').at(0));
			listen = ServerManager::split(serv.getListen(), ':');
			serv.setHost(listen.at(0));
			serv.setPort(std::atoi(listen.at(1).c_str()));
		}
		else if (splitted.at(0) == "server_name")
			serv.setName(ServerManager::split(splitted.at(1), ';').at(0));
		else if (splitted.at(0) == "error_page")
			serv.addErrorPage(std::atoi(splitted.at(1).c_str()), splitted.at(2));
		else if (splitted.at(0) == "port")
			serv.setPort(std::atoi(splitted.at(1).c_str()));
		else if (splitted.at(0) == "root")
			serv.setRoot(splitted.at(1));
		else if (splitted.at(0) == "location")
		{
			if (splitted.size() >= 3 && splitted.at(splitted.size() - 1) == "{")
			{
				if (splitted.size() > 3)
				{
					for (size_t i = 1; i < splitted.size() - 1; i++)
					{
						if (i > 1) 
							LocationRoot += " ";
						LocationRoot += splitted.at(i);
					}
				}
				else
					LocationRoot = splitted.at(1);
				this->setPosition(this->_ifs);
				this->_ifs.close();
				serv.addLocation(makeLocationBlock(LocationRoot));
				openFile();
				continue;
			}
			else
			{
				for (size_t i = 1; i < splitted.size(); i++)
				{
					if (i > 1) 
						LocationRoot += " ";
					LocationRoot += splitted.at(i);
				}
				_state = LOCATION_KEYWORD;
				if (this->_ifs.tellg() == -1)
					break ;
				continue;
			}
		}
		else if (splitted.at(0) == "}")
			break ;
		else
		{
			if (this->_ifs.tellg() == -1)
					break ;
			std::cout << " :ERROR Keyword(in server): " << splitted.at(0) << std::endl;
			throw SyntaxErrorException();
		}

	}
	_state = DEFAULT;
	this->_config.addServer(serv);
	this->setPosition(this->_ifs);
	this->_ifs.close();
}

Location*	FileParser::makeLocationBlock(std::string path)
{
	std::string					buf;
	std::string 				LocationRoot;
	Location*					locBlock;
	std::vector<std::string>	splitted;


	if (path == "/")
		locBlock = new LocationDefault();
	else if (path.find("api") != std::string::npos)
		locBlock = new LocationAPI();
	else if (path.find("download") != std::string::npos)
		locBlock = new LocationDownload();
	else if (path.find("upload") != std::string::npos)
		locBlock = new LocationUpload();
	else
		locBlock = new LocationCGI();
	
	locBlock->setPath(path);
	
	_state = IN_LOCATION;
	openFile();
	while (getline(this->_ifs, buf))
	{
		buf = trimBuf(buf);
		if (buf.empty())
			continue;
		
		splitted = preprocessToken(buf);
		std::cout << "Location: " << buf << std::endl;
		// size_t test = splitted.at(0) == "}";
		// std::cout << "Close test: " << test << std::endl;
		if (_state == LOCATION_KEYWORD)
		{
			if (splitted.size() == 1 && splitted.at(0) == "{")
			{
				this->setPosition(this->_ifs);
				this->_ifs.close();
				locBlock->addLocations(makeLocationBlock(LocationRoot));
				openFile();
			}
			else
				throw SyntaxErrorException();
		}
		if (splitted.at(0) == "}")
		{
			std::cout << "loc block end" << std::endl;
			break ;
		}
		if (splitted.at(0) == "location")
		{
			if (splitted.size() >= 3 && splitted.at(splitted.size() - 1) == "{")
			{
				if (splitted.size() > 3)
				{
					for (size_t i = 1; i < splitted.size() - 1; i++)
					{
						if (i > 1) 
							LocationRoot += " ";
						LocationRoot += splitted.at(i);
					}
				}
				else
				{
					LocationRoot = splitted.at(1);
				}
				this->setPosition(this->_ifs);
				this->_ifs.close();
				locBlock->addLocations(makeLocationBlock(LocationRoot));
				openFile();
			}
			else
			{
				for (size_t i = 1; i < splitted.size(); i++)
				{
					if (i > 1) 
						LocationRoot += " ";
					LocationRoot += splitted.at(i);
				}
				_state = LOCATION_KEYWORD;
			}
		}
		else if (splitted.at(0) == "root")
		{
			locBlock->setRoot(splitted.at(0));
			std::cout << "Root : " << locBlock->getRoot() << std::endl;
		}
		else if (splitted.at(0) == "index")
		{
			locBlock->setIndex(splitted.at(1));
			std::cout << "Index: "<<locBlock->getIndex() << std::endl;
		}
		else if (splitted.at(0) == "limit_except")
		{
			for (size_t i = 1; i < splitted.size(); i++)
				locBlock->addLimitExcept(splitted.at(i));
			std::cout << "limit except" << std::endl; 
		}
		else if (splitted.at(0) == "error_page")
			locBlock->addErrorPage(std::atoi(splitted.at(1).c_str()), splitted.at(2));
		else if (locBlock->getType() == API && splitted.at(0) == "return")
			locBlock->addReturn(std::atoi(splitted.at(1).c_str()), splitted.at(2));
		else if (locBlock->getType() == DOWNLOAD)
		{
			if (splitted.at(0) == "autoindex" && splitted.at(1) == "on")
				locBlock->setAutoIndex(true);
			else if (splitted.at(0) == "add_header")
				locBlock->addAddHeader(splitted.at(1), splitted.at(2));
		}
		else if (locBlock->getType() == UPLOAD)
		{
			if (splitted.at(0) == "client_body_temp_path")
				locBlock->setClientBodyTempPath(splitted.at(1));
			else if (splitted.at(0) == "client_body_in_file_only" && splitted.at(1) == "on")
				locBlock->setClientBodyFileOnly(true);
			else if (splitted.at(0) == "upload_store")
				locBlock->setUploadStore(splitted.at(1));
		}
		else if (locBlock->getType() == CGI)
		{
			if (splitted.at(0) == "fastcgi_pass")
				locBlock->setPass(splitted.at(1));
			else if (splitted.at(0) == "fastcgi_index")
				locBlock->setcgiIndex(splitted.at(1));
			else if (splitted.at(0) == "fastcgi_param")
				locBlock->addParam(splitted.at(1), splitted.at(2));
		}
		else
		{
			if (this->_ifs.tellg() == -1)
				break ;
			std::cout << "ERROR Keyword: " << splitted.at(0) << std::endl;
			throw SyntaxErrorException();
		}

	}
	_state = DEFAULT;
	this->setPosition(this->_ifs);
	this->_ifs.close();
	// std::cout << "Location " << locBlock->getPath() <<" Block closed" << std::endl;
	return (locBlock);
}


const char* FileParser::FileNotOpenedException::what() const throw()
{
	return ("You MotherFucker");
}

const char* FileParser::SyntaxErrorException::what() const throw()
{
	return ("You MotherFucker");
}