#pragma once
#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "11_ServerConfig.hpp"

enum ConnectionState {
    CONN_READING_HEADER,    // HTTP 헤더 읽는 중
    CONN_READING_BODY,      // HTTP 바디 읽는 중
    CONN_PROCESSING,        // 요청 처리 중
    CONN_WRITING_RESPONSE,  // 응답 전송 중
    CONN_KEEP_ALIVE,        // Keep-Alive 대기 중
    CONN_CLOSING            // 연결 종료 중
};

class ClientConnection {
    private:
        // 소켓 정보
        int             _socketFd;
        std::string     _clientIp;
        int             _clientPort;

        // 연결 상태
        ConnectionState _state;
        time_t          _lastActivity;
        time_t          _connectionStart;

        // HTTP 처리
        HttpRequest     _request;
        HttpResponse    _response;

        // 버퍼링
        std::string     _readBuffer;
        std::string     _writeBuffer;
        size_t          _writeOffset;        // 전송된 바이트 수

        // 서버 설정 참조
        const ServerConfig*   _serverConfig;

        // 내부 상수
        static const size_t READ_BUFFER_SIZE = 8192;    // 8KB
        static const size_t MAX_HEADER_SIZE = 8192;     // 8KB
        static const size_t WRITE_CHUNK_SIZE = 8192;    // 8KB

        // 내부 헬퍼 메서드들
        int     processHeader();
        int     processBody();
        bool    isHeaderComplete() const;
        void    updateLastActivity();
        void    resetForNextRequest();

    public:
        ClientConnection(int fd, const ServerConfig* config);
        ~ClientConnection();

        // 소켓 I/O
        int readData();
        int writeData();

        // 상태 관리
        ConnectionState getState() const { return _state; }
        void setState(ConnectionState new_state);

        // 타임아웃 관리
        bool isTimedOut(time_t current_time, int timeout_seconds) const;
        time_t getLastActivity() const { return _lastActivity; }
        time_t getConnectionDuration(time_t current_time) const;

        // 요청/응답 처리
        int processRequest();
        bool isRequestComplete() const;
        bool isResponseComplete() const;

        // Getters
        int getSocketFd() const { return _socketFd; }
        const HttpRequest& getRequest() const { return _request; }
        HttpResponse& getResponse() { return _response; }
        const ServerConfig* getServerConfig() const { return _serverConfig; }
        const std::string& getClientIP() const { return _clientIp; }
        int getClientPort() const { return _clientPort; }

        // 연결 정보
        std::string getConnectionInfo() const;

        // Keep-Alive 관리
        bool shouldKeepAlive() const;
        void prepareForNextRequest();

        // 에러 처리
        void setError(int error_code, const std::string& message = "");
        bool hasError() const;

        // 디버깅
        std::string getStateString() const;
        void logState() const;
};