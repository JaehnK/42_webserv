#include "StaticFileHandler.hpp"
#include "StringUtils.hpp"

int StaticFileHandler::handleRequest(ClientConnection& conn, const Location* location) {
    const HttpRequest& request = conn.getRequest();
    HttpResponse& response = conn.getResponse();
    
    // 파일 경로 구성
    std::string file_path = buildFilePath(location->getRoot(), request.getPath());
    
    // 디렉토리 탐색 공격 방지
    if (isDirectoryTraversalAttempt(request.getPath())) {
        response.setErrorPage(403, "Forbidden");
        return 0;
    }
    
    // 파일 존재 확인 (실제로는 FileUtils 사용)
    // 여기서는 간단한 구현
    response.setFile(file_path);
    return 0;
}

bool StaticFileHandler::canHandle(const HttpRequest& request, const Location* location) {
    return request.getMethod() == "GET" || request.getMethod() == "HEAD";
}

std::string StaticFileHandler::buildFilePath(const std::string& root, const std::string& path) const {
    std::string full_path = root;
    if (!StringUtils::endsWith(root, "/") && !StringUtils::startsWith(path, "/")) {
        full_path += "/";
    }
    full_path += path;
    return full_path;
}

bool StaticFileHandler::isDirectoryTraversalAttempt(const std::string& path) const {
    return StringUtils::contains(path, "../") || StringUtils::contains(path, "..\\");
}

std::string StaticFileHandler::getMimeType(const std::string& filepath) const {
    if (StringUtils::endsWith(filepath, ".html") || StringUtils::endsWith(filepath, ".htm")) {
        return "text/html";
    } else if (StringUtils::endsWith(filepath, ".css")) {
        return "text/css";
    } else if (StringUtils::endsWith(filepath, ".js")) {
        return "application/javascript";
    } else if (StringUtils::endsWith(filepath, ".png")) {
        return "image/png";
    } else if (StringUtils::endsWith(filepath, ".jpg") || StringUtils::endsWith(filepath, ".jpeg")) {
        return "image/jpeg";
    } else {
        return "application/octet-stream";
    }
}