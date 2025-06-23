#pragma once
#include "RequestHandler.hpp"

class UploadHandler : public RequestHandler {
    private:
        int         saveUploadedFile(const std::string& content, const std::string& upload_dir);
        std::string generateUniqueFilename(const std::string& upload_dir) const;

    public:
        virtual int     handleRequest(ClientConnection& conn, const Location* location);
        virtual bool    canHandle(const HttpRequest& request, const Location* location);
};