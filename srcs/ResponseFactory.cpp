#include "ResponseFactory.hpp"

IResponse* ResponseFactory::create(const HttpRequest& req)
{
    HttpMethod method = req.getMethod();

    switch (method)
    {
        case GET:
            return new GetResponse(req);
        // case POST:
        //     return new PostResponse(req);
        // case DELETE:
        //     return new DeleteResponse(req);
        default:
            throw std::runtime_error("Unsupported HTTP method");
    }
}