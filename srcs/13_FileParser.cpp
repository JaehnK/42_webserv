#include "webserv.hpp"

ParseContext::ParseContext(ContextType type, const std::string& path, Location* loc)
	: type(type), locationPath(path), locationPtr(NULL) {}


FileParser::FileParser()
{}

FileParser::FileParser(char *fileName)
{

	this->_fileName = fileName;
    this->_currentLine = 0;
	this->_contextStack = std::stack<ParseContext>();
    
    this->_config = Config();
    this->_currentServer = NULL;
    this->_currentLocation = NULL;

	parse();
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
		this->_contextStack = rhs.getContextStack();
	}
	return (*this);
}

FileParser::~FileParser()
{
}


std::string	FileParser::getFileName() const
{
	return (this->_fileName);
}

Config	FileParser::getConfig() const
{
	return (this->_config);
}

std::stack<ParseContext> FileParser::getContextStack() const
{
	return (this->_contextStack);
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
		return "";

	end = output.find_last_not_of("  \t");
	if (end == std::string::npos)
		end = output.length() - 1;
	
	output = output.substr(start, end - start + 1);
	return (output);
}

std::vector<std::string>	FileParser::preprocessToken(std::string& buf)
{
	char						lstChar;
	std::vector<std::string>	splitted; 

	lstChar = buf[buf.length() - 1];
	if (buf == "server" || buf.find("location") != std::string::npos)
		;
	else if (std::string("{};").find(lstChar) == std::string::npos)
		throw FileParserSyntaxErrorException("Invalid syntax: " + buf);
	
	
	splitted = ServerManager::split(buf, ' ');
	if (splitted.size() == 1 && (splitted.at(0) == "{" && splitted.at(0) == "}"))
        ;
	return (splitted);
}

void    FileParser::validateToken(const std::vector<std::string>& tokens) const
{
    std::string directive = tokens[0];
    std::string lastToken = tokens[tokens.size() - 1];
    if (tokens.empty())
        return ;
    
    if (directive != "server" && directive != "location" && \
        directive != "{" && directive != "}")
    {
        if (tokens.size() < 2)
            throw FileParserSyntaxErrorException("Directive Missing Value:" + directive);
        if (!lastToken.empty() && lastToken[lastToken.length() - 1] != ';')
            throw FileParserSyntaxErrorException("Missing Semicolon After Directive: " + directive);
    }
}

std::string FileParser::removeSemicolon(const std::string& str) const
{
    std::string result = str;
    if (!result.empty() && result[result.length() - 1] == ';')
    {
        result.erase(result.length() - 1);
    }
    return result;
}

std::string FileParser::extractLocationPath(const std::vector<std::string>& tokens) const
{
    std::string path;
    if (tokens.size() < 2) 
        throw FileParserSyntaxErrorException("Location directive requires a path");
    
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        if (tokens[i] == "{")
            break;
        if (i > 1)
            path += " ";
        path += tokens[i];
    }
    return (path);
}

int FileParser::parseMultiplier(std::string& value) const
{
    char lastChar;
    if (value.empty())
        throw FileParserSyntaxErrorException("Empty value in size directive");
    
    lastChar = value[value.length() - 1];
    if (lastChar == 'M' || lastChar == 'm')
    {
        value.erase(value.length() - 1);
        return 1000000;
    } 
    else if (lastChar == 'K' || lastChar == 'k')
    {
        value.erase(value.length() - 1);
        return 1000;
    }
    else if (std::isdigit(lastChar))
    {
        return 1;
    } 
    else
    {
        throw FileParserSyntaxErrorException("Invalid size unit");
    }
}

int FileParser::parseNumericValue(const std::string& str) const
{
    if (str.empty())
        throw FileParserSyntaxErrorException("Empty numeric value");
    
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (!std::isdigit(str[i]))
            throw FileParserSyntaxErrorException("Invalid numeric value: " + str);
    }
    return (std::atoi(str.c_str()));
}

void    FileParser::parse()
{
    std::string     line;
	std::ifstream   ifs(this->_fileName.c_str());
	if (!ifs.is_open())
		throw FileParserFileNotOpenedException(this->_fileName);
    
    this->_contextStack.push(ParseContext(ParseContext::GlOBAL));
    try
    {
        while (getline(ifs, line))
        {
            _currentLine++;
            processLine(line);
        }
        ifs.close();

        if (this->_contextStack.size() != 1)
            throw FileParserSyntaxErrorException("Unclosed blocks detected", _currentLine);

        afterParse();
    }
    catch (const std::exception& e)
    {
        ifs.close();
        afterParse();
        throw;
    }
	

}

void    FileParser::afterParse()
{
    while (!this->_contextStack.empty())
        this->_contextStack.pop();
    
    this->_currentServer = NULL;
    this->_currentLocation = NULL;
    this->_currentLine = 0;
}

void    FileParser::processLine(std::string& line)
{
    line = trimBuf(line);
    if (line.empty())
        return ;
    
    try
    {
        std::vector<std::string> tokens = preprocessToken(line);
        validateToken(tokens);
        processTokens(tokens);
    }
    catch (const std::exception& e)
    {
        throw FileParserSyntaxErrorException(std::string(e.what()), _currentLine);
    }
    
}

void    FileParser::processTokens(const std::vector<std::string>& tokens)
{
    if (tokens.empty())
        return ;
    
    const std::string& directive = tokens[0];

    if (directive == "}")
    {
        exitBlock();
        return ;
    }

    ParseContext::ContextType current = this->_contextStack.top().type;

    switch (current)
    {
        case ParseContext::GlOBAL:
            processGlobalDirective(tokens);
            break;
        
        case ParseContext::SERVER:
            processServerDirective(tokens);
            break;
    
        case ParseContext::LOCATION:
            processLocationDirective(tokens);
            break;
        
        default:
            break;
    }
}
 

void    FileParser::processGlobalDirective(const std::vector<std::string>& tokens)
{
    const std::string& dir = tokens[0];

    if (dir == "client_max_body_size")
        processClientMaxBodySize(tokens);
    else if (dir == "server")
        enterServerBlock();
    else
        throw FileParserSyntaxErrorException("Unknown Global Directive: " + dir);
}

void    FileParser::processServerDirective(const std::vector<std::string>& tokens)
{
    const std::string& dir = tokens[0];

    if (dir == "listen")
    {
        if (tokens.size() < 2)
            throw FileParserSyntaxErrorException("listen requires a value");
        processListenDirective(tokens[1]);
    }
    else if (dir == "server_name")
    {
        if (tokens.size() < 2)
            throw FileParserSyntaxErrorException("server_name requires a value");
        this->_currentServer->setName(removeSemicolon(tokens[1]));
    }
    else if (dir == "error_page")
    {
        processErrorPageDirective(tokens);
    }
    else if (dir == "root")
    {
        if (tokens.size() < 2)
            throw FileParserSyntaxErrorException("root requires a value");
        _currentServer->setRoot(removeSemicolon(tokens[1]));
    }
    else if (dir == "location")
    {
        enterLocationBlock(extractLocationPath(tokens));
    }
    else
        throw FileParserSyntaxErrorException("Unknown Server Directive: " + dir);
}

void    FileParser::processLocationDirective(const std::vector<std::string>& tokens)
{
    const std::string&  dir = tokens[0];
    locationType        type = this->_currentLocation->getType();

    if (dir == "root")
    {
        if (tokens.size() < 2)
            throw FileParserSyntaxErrorException("root requires a value");
        this->_currentLocation->setRoot(removeSemicolon(tokens[1]));
        return ;
    }
    else if (dir == "limit_except")
    {
        if (tokens.size() < 2)
            throw FileParserSyntaxErrorException("limit_except requires a value");
        for (int i = 1; i < tokens.size(); i++)
        {
            if (i == tokens.size())
                this->_currentLocation->addLimitExcept(removeSemicolon(tokens[i]));
            else
                this->_currentLocation->addLimitExcept(tokens[i]);
        }
        return ;
    }
    else if (dir == "index")
    {
        if (tokens.size() < 2)
            throw FileParserSyntaxErrorException("index requires a value");
        this->_currentLocation->setIndex(removeSemicolon(tokens[1]));
        return ;
    }

    if (type == API && dir == "return")
    {
        if (dir == "return")
        {
            if (tokens.size() < 3)
                throw FileParserSyntaxErrorException("return requires code and URL");
            this->_currentLocation->addReturn(parseNumericValue(tokens[1]), \
                                        removeSemicolon(tokens[2]));
        }
        else
            throw FileParserSyntaxErrorException("Unkown Directive in Location API: " + dir);            
    }
    else if (type == DOWNLOAD)
    {
        if (dir == "autoindex")
        {
            if (removeSemicolon(tokens[1]) != "on")
                throw FileParserSyntaxErrorException("autoindex has invalid value: " + removeSemicolon(tokens[1]));
            this->_currentLocation->setAutoIndex(true);
        }
        else if (dir == "add_header")
        {
            if (tokens.size() < 3)
                throw FileParserSyntaxErrorException("add_header requires key and value");
            this->_currentLocation->addAddHeader(tokens[1], removeSemicolon(tokens[2]));
        }
        else
            throw FileParserSyntaxErrorException("Unkown Directive in Location Download: " + dir);
    }
    else if (type == UPLOAD)
    {
        if (dir == "client_body_temp_path")
        {
            if (tokens.size() < 2)
                throw FileParserSyntaxErrorException("client_body_path requires a value");
            this->_currentLocation->setClientBodyTempPath(tokens[1]);
        }
        else if (dir == "client_body_in_file_only")
        {
            if (tokens.size() < 2)
                throw FileParserSyntaxErrorException("client_body_in_file_only requires a value");
            if (removeSemicolon(tokens[1]) != "on")
                throw FileParserSyntaxErrorException("client_body_in_file_only has invalid value: " + removeSemicolon(tokens[1]));
            this->_currentLocation->setClientBodyFileOnly(true);
        }
        else if (dir == "upload_store")
        {
            if (tokens.size() < 2)
                throw FileParserSyntaxErrorException("upload_store requires a value");
            this->_currentLocation->setUploadStore(removeSemicolon(tokens[1]));
        }
    }
    else if (type == CGI)
    {
        if (dir == "fastcgi_pass")
        {
            if (tokens.size() < 2)
                throw FileParserSyntaxErrorException("fastcgi_pass requires a value");
            this->_currentLocation->setPass(removeSemicolon(tokens[1]));
        }
        else if (dir == "fastcgi_index")
        {
            if (tokens.size() < 2)
                throw FileParserSyntaxErrorException("fastcgi_index requires a value");
            this->_currentLocation->setPass(removeSemicolon(tokens[1]));
        }
        else if (dir == "fastcgi_param")
        {
            if (tokens.size() < 3)
                throw FileParserSyntaxErrorException("fastcgi_param requires a value");
            this->_currentLocation->addParam(tokens[1], removeSemicolon(tokens[2]));
        }
    }
    else
        throw FileParserSyntaxErrorException("Unkown Directive for location type: " + dir);
}


void    FileParser::processClientMaxBodySize(const std::vector<std::string>& tokens)
{
    if (tokens.size() != 2)
        throw FileParserSyntaxErrorException("client_max_body_size requires exactly one argument");
    
    std::string value = removeSemicolon(tokens[1]);
    int multiplier = parseMultiplier(value);
    int size = parseNumericValue(value) * multiplier;
    
    _config.setClientMaxBodySize(size);
}

void    FileParser::processListenDirective(const std::string& value)
{
    std::string listen = removeSemicolon(value);
    this->_currentServer->setListen(listen);
    
    std::vector<std::string> hostPort = ServerManager::split(listen, ':');
    if (hostPort.size() == 2)
    {
        _currentServer->setHost(hostPort[0]);
        _currentServer->setPort(parseNumericValue(hostPort[1]));
    }
}

void    FileParser::processErrorPageDirective(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 3)
        throw FileParserSyntaxErrorException("error_page requires code and path");
    
    int code = parseNumericValue(tokens[1]);
    std::string path = removeSemicolon(tokens[2]);
    _currentServer->addErrorPage(code, path);
}

void    FileParser::enterServerBlock()
{
    this->_contextStack.push(ParseContext(ParseContext::SERVER));
    this->_currentServer = new ServerConfig();
}

void    FileParser::enterLocationBlock(const std::string& path)
{
    Location* newLocation = createLocation(path);

    this->_contextStack.push(ParseContext(ParseContext::LOCATION, path, newLocation));
    this->_currentLocation = newLocation;
    this->_currentLocation->setPath(path);
}

void    FileParser::exitBlock()
{
    if (this->_contextStack.size() <= 1)
        throw FileParserSyntaxErrorException("Unexpected '}'");
    
    ParseContext context = this->_contextStack.top();
    this->_contextStack.pop();

    switch (context.type)
    {
        case ParseContext::SERVER:
            this->_config.addServer(*_currentServer);
            delete (_currentServer);
            this->_currentServer = NULL;
            break;
        
        case ParseContext::LOCATION:
            if (this->_contextStack.top().type == ParseContext::SERVER)
            {
                this->_currentServer->addLocation(this->_currentLocation);
                this->_currentLocation = NULL;
            }
            else if (this->_contextStack.top().type == ParseContext::LOCATION)
            {
                Location* parentLoc = this->_contextStack.top().locationPtr;
                if (!parentLoc)
                    throw FileParserSyntaxErrorException("Parent Location Not Found");
                parentLoc->addLocations(this->_currentLocation);
                this->_currentLocation = parentLoc;
            }
            break;
        
        case ParseContext::GlOBAL:
            throw FileParserSyntaxErrorException("Unexpected '}'");
    }
}

Location*   FileParser::createLocation(const std::string& path) const
{
    if (path == "/")
        return new LocationDefault();
    else if (path.find("api") != std::string::npos)
        return new LocationAPI();
    else if (path.find("download") != std::string::npos)
        return new LocationDownload();
    else if (path.find("upload") != std::string::npos)
        return new LocationUpload();
    else
        return new LocationCGI();
    
    return (NULL);
}


FileParser::FileParserFileNotOpenedException::FileParserFileNotOpenedException(const std::string &filename)
{
    this->_msg = "Failed to open configuration file: " + filename;
}

FileParser::FileParserSyntaxErrorException::~FileParserSyntaxErrorException() throw()
{}

const char* FileParser::FileParserFileNotOpenedException::what() const throw()
{
	return ("You MotherFucker");
}

FileParser::FileParserSyntaxErrorException::FileParserSyntaxErrorException(const std::string &msg, int line)
{
    _msg = msg;
    if (line > 0)
    {
        std::ostringstream oss;
        oss << " at line " << line;
        this->_msg += oss.str();
    }
}

const char* FileParser::FileParserSyntaxErrorException::what() const throw()
{
	return (this->_msg.c_str());
}

FileParser::FileParserFileNotOpenedException::~FileParserFileNotOpenedException() throw()
{}