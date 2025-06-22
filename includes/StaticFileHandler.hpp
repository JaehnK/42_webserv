#include "RequestHandler.hpp"

class StaticFileHandler : public RequestHandler {
    private:
        std::string getMimeType(const std::string& filepath) const;
        std::string buildFilePath(const std::string& root, const std::string& path) const;
        bool        isDirectoryTraversalAttempt(const std::string& path) const;
        std::string generateDirectoryListing(const std::string& dirpath, const std::string& request_path) const;
        
    public:
        virtual int     handleRequest(ClientConnection& conn, const Location* location);
        virtual bool    canHandle(const HttpRequest& request, const Location* location);
};