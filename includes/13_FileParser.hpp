#pragma once
#include "webserv.hpp"

class	FileParser
{
	private:
		Config			_config;
		std::string		_fileName;
		std::streampos	_currentPos;
		std::ifstream	_ifs;
		size_t			_state;

		enum State
		{
			DEFAULT = 0,
			SERVER_KEYWORD = 1,
			IN_SERVER = 2,
			LOCATION_KEYWORD = 3,
			IN_LOCATION = 4,
		};

		FileParser();
		std::string trimBuf(std::string &buf) const;
		
	public:
		FileParser(char *fileName);
		FileParser(const FileParser& rhs);
		FileParser& operator=(const FileParser &rhs);
		~FileParser();

		void		setFileName(const std::string fileName);
		void		setPosition(std::ifstream &ifs);
		
		std::string		getFileName() 	const;
		std::streampos	getPosition() 	const;
		Config			getConfig()		const;

		void						openFile();
		void						Parse();
		std::vector<std::string>	preprocessToken(std::string& buf);
		void						makeServerBlock();
		Location*					makeLocationBlock(std::string root);
		


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
