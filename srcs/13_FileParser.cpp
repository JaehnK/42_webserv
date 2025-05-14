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
		_ifs.seekg(this->_currentPos);
}

void    FileParser::Parse()
{
	std::ifstream	ifs;
	std::string		buf;
	int				brackets;
	char			lstChar;

	openFile();
	buf.clear();
	brackets = 0;
	this->config = Config();
	while (getline(this->_ifs, buf))
	{
		buf = trimBuf(buf);
		if (buf.empty())
			continue;
		
		std::cout << buf << std::endl;

		lstChar = buf[buf.length() - 1];
		if (std::string("{};").find(lstChar) == std::string::npos)
			throw SyntaxErrorException();
		
		std::vector<std::string>	splitted = ServerManager::split(buf, ' ');
		if (splitted.size() == 1 && (splitted.at(0) =="{" || splitted.at(0) =="}"))
			throw SyntaxErrorException();
		
		if (splitted.at(0) == "client_max_body_size")
		{
			std::string	cmds = splitted.at(1);
			int			digit;
			int			clientMaxBodySize = 0;

			if (cmds[splitted.at(1).length() - 1] != ';')
				throw SyntaxErrorException();
			else
				cmds.erase(cmds.length() - 1);
			
			if (cmds[splitted.at(1).length() - 1] == 'M' || \
					cmds[splitted.at(1).length() - 1] == 'm')
				digit = 1000000;
			else if (cmds[splitted.at(1).length() - 1] == 'K' || \
				cmds[splitted.at(1).length() - 1] == 'k')
				digit = 1000;
			else
				throw SyntaxErrorException();
			cmds.erase(cmds.length() - 1);

			for (std::string::iterator it = cmds.begin(); it != cmds.end(); ++it)
			{
				if (std::isdigit(*it) == false)
					throw SyntaxErrorException();
			}
			clientMaxBodySize = std::atoi(cmds.c_str());
			clientMaxBodySize *= digit;
			this->config.setClientMaxBodySize(clientMaxBodySize);
		}
		
		buf.clear();
	}
}

const char* FileParser::FileNotOpenedException::what() const throw()
{
	return ("You MotherFucker");
}

const char* FileParser::SyntaxErrorException::what() const throw()
{
	return ("You MotherFucker");
}