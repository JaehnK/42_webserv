#include "RequestHandler.hpp"

class CgiHandler : public RequestHandler {
    private:
        int                         executeCgi(const std::string& script_path, const HttpRequest& request, HttpResponse& response);
        std::vector<std::string>    buildCgiEnv(const HttpRequest& request, const std::string& script_path);
        
    public:
        virtual int     handleRequest(ClientConnection& conn, const Location* location);
        virtual bool    canHandle(const HttpRequest& request, const Location* location);
};
