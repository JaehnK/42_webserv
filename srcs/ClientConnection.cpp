#include "ClientConnection.hpp"
#include "StringUtils.hpp"

ClientConnection::ClientConnection(int fd, const ServerConfig* config) 
    : _socketFd(fd), _clientPort(0), _state(CONN_READING_HEADER), 
      _writeOffset(0), _serverConfig(config) {
    
    time_t now = time(NULL);
    _lastActivity = now;
    _connectionStart = now;
    
    // 클라이언트 주소 정보 가져오기
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    if (getpeername(fd, (struct sockaddr*)&client_addr, &addr_len) == 0) {
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
        _clientIp = ip_str;
        _clientPort = ntohs(client_addr.sin_port);
    } else {
        _clientIp = "unknown";
        _clientPort = 0;
    }
}

ClientConnection::~ClientConnection() {
    if (_socketFd != -1) {
        close(_socketFd);
    }
}

int ClientConnection::readData() {
    updateLastActivity();
    
    char buffer[READ_BUFFER_SIZE];
    ssize_t bytes_read = recv(_socketFd, buffer, READ_BUFFER_SIZE - 1, 0);
    
    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // 논블로킹 소켓에서 데이터 없음
        }
        return -1;  // 실제 에러
    }
    
    if (bytes_read == 0) {
        return -1;  // 연결 종료
    }
    
    buffer[bytes_read] = '\0';
    _readBuffer.append(buffer, bytes_read);
    
    // 상태에 따라 처리
    if (_state == CONN_READING_HEADER) {
        return processHeader();
    } else if (_state == CONN_READING_BODY) {
        return processBody();
    }
    
    return 0;
}

int ClientConnection::writeData() {
    updateLastActivity();
    
    if (_writeBuffer.empty()) {
        return 0;  // 전송할 데이터 없음
    }
    
    size_t remaining = _writeBuffer.length() - _writeOffset;
    size_t to_send = (remaining > WRITE_CHUNK_SIZE) ? WRITE_CHUNK_SIZE : remaining;
    
    ssize_t bytes_sent = send(_socketFd, 
                             _writeBuffer.c_str() + _writeOffset, 
                             to_send, 0);
    
    if (bytes_sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // 논블로킹 소켓에서 전송 버퍼 가득참
        }
        return -1;  // 실제 에러
    }
    
    _writeOffset += bytes_sent;
    
    // 전송 완료 확인
    if (_writeOffset >= _writeBuffer.length()) {
        _writeBuffer.clear();
        _writeOffset = 0;
        
        // 응답 전송 완료
        if (_state == CONN_WRITING_RESPONSE) {
            if (shouldKeepAlive()) {
                prepareForNextRequest();
            } else {
                setState(CONN_CLOSING);
            }
        }
    }
    
    return bytes_sent;
}

void ClientConnection::setState(ConnectionState new_state) {
    _state = new_state;
    updateLastActivity();
}

bool ClientConnection::isTimedOut(time_t current_time, int timeout_seconds) const {
    return (current_time - _lastActivity) > timeout_seconds;
}

time_t ClientConnection::getConnectionDuration(time_t current_time) const {
    return current_time - _connectionStart;
}

int ClientConnection::processRequest() {
    setState(CONN_PROCESSING);
    
    // 기본 응답 설정
    _response.setStatus(200);
    _response.setHeader("Server", "WebServer/1.0");
    
    // 요청 검증
    if (_request.getMethod().empty() || _request.getPath().empty()) {
        setError(400, "Bad Request");
        return -1;
    }
    
    // Content-Length 검증 (POST 요청의 경우)
    if (_request.getMethod() == "POST" || _request.getMethod() == "PUT") {
        if (!_request.hasHeader("content-length")) {
            setError(411, "Length Required");
            return -1;
        }
    }
    
    // 응답을 write_buffer에 준비
    _writeBuffer = _response.toString();
    _writeOffset = 0;
    
    setState(CONN_WRITING_RESPONSE);
    return 0;
}

bool ClientConnection::isRequestComplete() const {
    return _request.isComplete();
}

bool ClientConnection::isResponseComplete() const {
    return _writeBuffer.empty() || _writeOffset >= _writeBuffer.length();
}

std::string ClientConnection::getConnectionInfo() const {
    return _clientIp + ":" + StringUtils::toString(_clientPort);
}

bool ClientConnection::shouldKeepAlive() const {
    // HTTP/1.1은 기본적으로 keep-alive
    if (_request.getVersion() == "HTTP/1.1") {
        std::string connection = StringUtils::toLower(_request.getHeader("connection"));
        return connection != "close";
    }
    
    // HTTP/1.0은 기본적으로 close
    std::string connection = StringUtils::toLower(_request.getHeader("connection"));
    return connection == "keep-alive";
}

void ClientConnection::prepareForNextRequest() {
    resetForNextRequest();
    setState(CONN_KEEP_ALIVE);
}

void ClientConnection::setError(int error_code, const std::string& message) {
    _response.setErrorPage(error_code, "");
    if (!message.empty()) {
        _response.setBody(message);
    }
    
    _response.setHeader("Connection", "close");
    _writeBuffer = _response.toString();
    _writeOffset = 0;
    setState(CONN_WRITING_RESPONSE);
}

bool ClientConnection::hasError() const {
    return _response.getStatusCode() >= 400;
}

std::string ClientConnection::getStateString() const {
    switch (_state) {
        case CONN_READING_HEADER:    return "READING_HEADER";
        case CONN_READING_BODY:      return "READING_BODY";
        case CONN_PROCESSING:        return "PROCESSING";
        case CONN_WRITING_RESPONSE:  return "WRITING_RESPONSE";
        case CONN_KEEP_ALIVE:        return "KEEP_ALIVE";
        case CONN_CLOSING:           return "CLOSING";
        default:                     return "UNKNOWN";
    }
}

void ClientConnection::logState() const {
    // 추후 Logger 클래스 구현 시 사용
    // Logger::debug("Connection " + getConnectionInfo() + " state: " + getStateString());
}

// ================================
// 내부 헬퍼 메서드들
// ================================

int ClientConnection::processHeader() {
    // 헤더 완료 확인 (\r\n\r\n 찾기)
    size_t header_end = _readBuffer.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        // 헤더가 너무 큰지 확인
        if (_readBuffer.length() > MAX_HEADER_SIZE) {
            setError(413, "Request Header Too Large");
            return -1;
        }
        return 0;  // 더 읽어야 함
    }
    
    // 헤더 파싱
    std::string header_data = _readBuffer.substr(0, header_end + 4);
    if (_request.parseRequest(header_data) != 0) {
        setError(400, "Bad Request");
        return -1;
    }
    
    // 읽은 헤더 부분을 버퍼에서 제거
    _readBuffer.erase(0, header_end + 4);
    
    // 바디가 있는지 확인
    if (_request.hasBody()) {
        setState(CONN_READING_BODY);
        return processBody();  // 이미 읽은 바디 데이터가 있을 수 있음
    } else {
        return processRequest();  // 바로 처리
    }
}

int ClientConnection::processBody() {
    // 남은 바디 데이터를 요청에 추가
    if (!_readBuffer.empty()) {
        _request.appendData(_readBuffer);
        _readBuffer.clear();
    }
    
    // 바디 완료 확인
    if (_request.isComplete()) {
        return processRequest();
    }
    
    return 0;  // 더 읽어야 함
}

bool ClientConnection::isHeaderComplete() const {
    return _readBuffer.find("\r\n\r\n") != std::string::npos;
}

void ClientConnection::updateLastActivity() {
    _lastActivity = time(NULL);
}

void ClientConnection::resetForNextRequest() {
    _request.reset();
    _response.reset();
    _readBuffer.clear();
    _writeBuffer.clear();
    _writeOffset = 0;
}