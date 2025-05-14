#pragma once
#include "webserv.hpp"

class FileParser
{
	private:
		Config			config;
		std::string		_fileName;
		std::streampos	_currentPos;
		std::ifstream	_ifs;
		
		FileParser();
		std::string trimBuf(std::string &buf) const;
		
	public:
		FileParser(char *fileName);
		FileParser(const FileParser& rhs);
		FileParser& operator=(const FileParser &rhs);
		~FileParser();

		void		setFileName(const std::string fileName);
		void		setPosition(std::ifstream &ifs);
		std::string		getFileName() const;
		std::streampos	getPosition() const;

		void	openFile();
		void    Parse();
		


		class FileNotOpenedException: public std::exception
		{
			public:
				const char* what() const throw();
		};
		class SyntaxErrorException: public std::exception
		{
			public:
				const char* what() const throw();
		};
};
