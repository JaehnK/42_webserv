#include "Server.hpp"
#include "StringUtils.hpp"

Server::Server(const ServerConfig& cfg) 
{
    config = cfg;
    listen_socket = -1;
    is_initialized = false;
    static_handler = NULL;
    cgi_handler = NULL;
    upload_handler = NULL;
}

Server::~Server()
{
    cleanup();
}

int Server::initialize()
{
    if (is_initialized)
        return 0;  // 이미 초기화됨

    // 핸들러들 생성
    static_handler = new(std::nothrow) StaticFileHandler();
    cgi_handler = new(std::nothrow) CgiHandler();
    upload_handler = new(std::nothrow) UploadHandler();
    
    if (!static_handler || !cgi_handler || !upload_handler)
    {
        cleanup();
        return -1;  // 메모리 할당 실패
    }
    
    // 소켓 생성 및 설정
    if (createListenSocket() != 0)
    {
        cleanup();
        return -1;
    }
    
    if (setSocketOptions() != 0)
    {
        cleanup();
        return -1;
    }
    
    if (bindSocket() != 0)
    {
        cleanup();
        return -1;
    }
    
    if (startListening() != 0)
    {
        cleanup();
        return -1;
    }
    
    is_initialized = true;
    logServerInfo();
    
    return 0;
}

void Server::cleanup()
{
    // 모든 클라이언트 연결 종료
    closeAllConnections();
    
    // 리슨 소켓 닫기
    if (listen_socket != -1)
    {
        close(listen_socket);
        listen_socket = -1;
    }
    
    // 핸들러들 해제
    delete static_handler;
    delete cgi_handler;
    delete upload_handler;
    static_handler = NULL;
    cgi_handler = NULL;
    upload_handler = NULL;
    is_initialized = false;
}

int Server::acceptConnection()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(listen_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;  // 논블로킹에서 연결 없음
        return -1;  // 실제 에러
    }
    
    // 클라이언트 소켓을 논블로킹으로 설정
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags != -1)
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    
    // ClientConnection 생성
    ClientConnection* conn = new(std::nothrow) ClientConnection(client_fd, &config);
    if (!conn)
    {
        close(client_fd);
        return -1;  // 메모리 할당 실패
    }
    
    // 연결 맵에 추가
    connections[client_fd] = conn;
    
    // 통계 업데이트
    stats.total_connections++;
    stats.current_connections++;
    
    return client_fd;  // 새 클라이언트 fd 반환
}

void Server::closeConnection(int client_fd)
{
    std::map<int, ClientConnection*>::iterator it = connections.find(client_fd);
    if (it != connections.end())
    {
        delete it->second;
        connections.erase(it);
        stats.current_connections--;
    }
}

void Server::closeAllConnections()
{
    for (std::map<int, ClientConnection*>::iterator it = connections.begin();
         it != connections.end(); ++it)
    {
        delete it->second;
    }
    connections.clear();
    stats.current_connections = 0;
}

ClientConnection* Server::getConnection(int fd)
{
    std::map<int, ClientConnection*>::iterator it = connections.find(fd);
    return (it != connections.end()) ? it->second : NULL;
}

int Server::handleClientRead(int client_fd)
{
    ClientConnection* conn = getConnection(client_fd);
    if (!conn)
        return -1;  // 연결을 찾을 수 없음
    
    int result = conn->readData();
    if (result < 0)
        return -1;  // 연결 에러 또는 종료
    
    // 요청이 완료되면 처리
    if (conn->isRequestComplete())
        return processClientRequest(conn);
    
    return 0;  // 계속 읽기 필요
}

int Server::handleClientWrite(int client_fd)
{
    ClientConnection* conn = getConnection(client_fd);
    if (!conn)
        return -1;
    
    int result = conn->writeData();
    if (result < 0)
        return -1;  // 연결 에러
    
    // 응답 전송 완료 시 상태에 따라 처리
    if (conn->isResponseComplete())
    {
        if (conn->getState() == CONN_CLOSING)
            return -1;  // 연결 종료
        // Keep-Alive의 경우 읽기 모드로 전환
    }
    
    return 0;
}

int Server::processClientRequest(ClientConnection* conn)
{
    const HttpRequest& request = conn->getRequest();
    HttpResponse& response = conn->getResponse();
    
    // 통계 업데이트
    stats.total_requests++;

    // Location 매칭
    const Location* location = matchLocation(request.getPath());
    if (!location)
    {
        conn->setError(404, "Not Found");
        return 0;
    }

    // HTTP 메서드 검증
    // if (!location->isMethodAllowed(request.getMethod()))
    // {
    //     conn->setError(405, "Method Not Allowed");
    //     return 0;
    // }
    
    // 적절한 핸들러 선택 및 실행
    RequestHandler* handler = selectHandler(request);
    if (!handler || !handler->canHandle(request, location))
    {
        conn->setError(500, "Internal Server Error");
        return 0;
    }
    
    return handler->handleRequest(*conn, location);
}

RequestHandler* Server::selectHandler(const HttpRequest& request)
{
    const std::string& path = request.getPath();
    
    // CGI 처리 (PHP 파일)
    if (StringUtils::endsWith(path, ".php"))
        return cgi_handler;
    
    // 업로드 처리
    if (StringUtils::startsWith(path, "/upload") && 
        (request.getMethod() == "POST" || request.getMethod() == "PUT"))
        return upload_handler;
    
    // 기본적으로 정적 파일 핸들러
    return static_handler;
}

const Location* Server::matchLocation(const std::string& path) const
{
    return config.matchLocation(path);
}

void Server::cleanupTimedOutConnections()
{
    std::vector<int> timed_out = getTimedOutConnections();
    
    for (std::vector<int>::iterator it = timed_out.begin(); 
         it != timed_out.end(); ++it)
{
        closeConnection(*it);
    }
}

std::vector<int> Server::getTimedOutConnections() const
{
    std::vector<int> timed_out;
    time_t now = time(NULL);
    
    for (std::map<int, ClientConnection*>::const_iterator it = connections.begin();
         it != connections.end(); ++it)
{
        if (it->second->isTimedOut(now, CONNECTION_TIMEOUT))
{
            timed_out.push_back(it->first);
        }
    }
    
    return timed_out;
}

std::string Server::getServerInfo() const
{
    return "Server " + config.getName() + " listening on " + getListenAddress();
}

std::string Server::getListenAddress() const
{
    return config.getHost() + ":" + StringUtils::toString(config.getPort());
}

bool Server::needsWriteEvent(int client_fd)
{
    ClientConnection* conn = getConnection(client_fd);
    if (!conn) return false;
    
    ConnectionState state = conn->getState();
    return (state == CONN_WRITING_RESPONSE);
}

bool Server::needsReadEvent(int client_fd)
{
    ClientConnection* conn = getConnection(client_fd);
    if (!conn) return false;
    
    ConnectionState state = conn->getState();
    return (state == CONN_READING_HEADER || 
            state == CONN_READING_BODY || 
            state == CONN_KEEP_ALIVE);
}

// ================================
// 내부 헬퍼 메서드들
// ================================

int Server::createListenSocket()
{
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0)
    {
        return -1;
    }
    
    // 논블로킹 모드 설정
    int flags = fcntl(listen_socket, F_GETFL, 0);
    if (flags != -1)
    {
        fcntl(listen_socket, F_SETFL, flags | O_NONBLOCK);
    }
    
    return 0;
}

int Server::setSocketOptions()
{
    // SO_REUSEADDR 설정 (포트 재사용 허용)
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, 
                   &SO_REUSEADDR_ENABLE, sizeof(SO_REUSEADDR_ENABLE)) < 0)
    {
        return -1;
    }
    
    return 0;
}

int Server::bindSocket()
{
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.getPort());
    
    // IP 주소 설정
    if (inet_pton(AF_INET, config.getHost().c_str(), &server_addr.sin_addr) <= 0)
    {
        return -1;
    }
    
    if (bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        return -1;
    }
    
    return 0;
}

int Server::startListening()
{
    if (listen(listen_socket, LISTEN_BACKLOG) < 0)
        return -1;
    return 0;
}

void Server::logServerInfo() const
{
    // 추후 Logger 클래스 구현 시 사용
    // Logger::info(getServerInfo());
}