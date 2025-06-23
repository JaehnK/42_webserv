#pragma once
#include "webserv.hpp"


struct	ParseContext
{
	enum ContextType
	{
		GlOBAL = 0,
		SERVER = 1,
		LOCATION = 2
	};

	ContextType	type;
	std::string	locationPath;
    Location*   locationPtr;

	ParseContext(ContextType type, const std::string& path = "", Location* loc = NULL);
};


class	FileParser
{
	private:
		std::string					_fileName;
        int                         _currentLine;
		std::stack<ParseContext>	_contextStack;
        
        Config                      _config;
        ServerConfig*               _currentServer;
        Location*                   _currentLocation;

		FileParser();

        // utils
		std::string                 trimBuf(std::string &buf) const;
        std::vector<std::string>	preprocessToken(std::string& buf);
        void                        validateToken(const std::vector<std::string>& tokens) const;
        std::string                 removeSemicolon(const std::string& str) const;
        std::string                 extractLocationPath(const std::vector<std::string>& tokens) const;
        int                         parseMultiplier(std::string& value) const;
        int                         parseNumericValue(const std::string& str) const;
        Location*                   createLocation(const std::string& path) const;
		
	public:
		FileParser(char *fileName);
		FileParser(const FileParser& rhs);
		FileParser& operator=(const FileParser &rhs);
		~FileParser();
		
		std::string					getFileName() 		const;
		Config  					getConfig()		const;
		std::stack<ParseContext>	getContextStack()	const;	

		void						parse();
        void                        afterParse();

        void                        processLine(std::string& line);
        void                        processTokens(const std::vector<std::string> &tokens);

        void                        processGlobalDirective(const std::vector<std::string>& tokens);
        void                        processServerDirective(const std::vector<std::string>& tokens);
        void                        processLocationDirective(const std::vector<std::string>& tokens);

        void                        enterServerBlock();
        void                        enterLocationBlock(const std::string& path);
        void                        exitBlock();

        void                        processClientMaxBodySize(const std::vector<std::string> &tokens);
        void                        processListenDirective(const std::string& value);
        void                        processErrorPageDirective(const std::vector<std::string>& tokens);




		class FileParserFileNotOpenedException: public std::exception
		{
            private:
                std::string _msg;

			public:
                FileParserFileNotOpenedException(const std::string& filename);
				virtual ~FileParserFileNotOpenedException() throw();
                const char* what() const throw();
		};
		class FileParserSyntaxErrorException: public std::exception
		{
            private:
                std::string _msg;
            
			public:
                FileParserSyntaxErrorException(const std::string &msg, int line = -1);
				virtual ~FileParserSyntaxErrorException() throw();
                const char* what() const throw();
		};
};
