#pragma once

#include "ClientConnection.hpp"

class RequestHandler {
    public:
        virtual ~RequestHandler() {}
        virtual int handleRequest(ClientConnection& conn, const Location* location) = 0;
        virtual bool canHandle(const HttpRequest& request, const Location* location) = 0;
};