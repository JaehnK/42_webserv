#pragma once
#include <map>
#include <string>
#include "11_ServerConfig.hpp"
#include "ClientConnection.hpp"
#include "StaticFileHandler.hpp"

class Server {
    private:
        // 설정 및 소켓
        ServerConfig config;
        int listen_socket;
        bool is_initialized;
        
        // 클라이언트 연결 관리
        std::map<int, ClientConnection*> connections;
        
        // 요청 핸들러들
        StaticFileHandler* static_handler;
        // CgiHandler* cgi_handler;
        // UploadHandler* upload_handler;
        
        // 통계
        struct ServerStats {
            size_t total_connections;
            size_t current_connections;
            size_t total_requests;
            time_t start_time;
            
            ServerStats() : total_connections(0), current_connections(0), 
                           total_requests(0), start_time(time(NULL)) {}
        } stats;
        
        // 상수
        static const int LISTEN_BACKLOG = 128;
        static const int SO_REUSEADDR_ENABLE = 1;
        static const int CONNECTION_TIMEOUT = 60;  // 60초
        
        // 내부 헬퍼 메서드들
        int createListenSocket();
        int bindSocket();
        int startListening();
        int setSocketOptions();
        void logServerInfo() const;
        
    public:
        Server(const ServerConfig& cfg);
        ~Server();
        
        // 서버 초기화 및 정리
        int initialize();
        void cleanup();
        
        // 연결 관리
        int acceptConnection();
        void closeConnection(int client_fd);
        void closeAllConnections();
        ClientConnection* getConnection(int fd);
        
        // 요청 처리
        int handleClientRead(int client_fd);
        int handleClientWrite(int client_fd);
        int processClientRequest(ClientConnection* conn);
        
        // 핸들러 선택
        RequestHandler* selectHandler(const HttpRequest& request);
        const Location* matchLocation(const std::string& path) const;
        
        // 정리 작업
        void cleanupTimedOutConnections();
        std::vector<int> getTimedOutConnections() const;
        
        // Getters
        int getListenSocket() const { return listen_socket; }
        const ServerConfig& getConfig() const { return config; }
        size_t getConnectionCount() const { return connections.size(); }
        const ServerStats& getStats() const { return stats; }
        bool isInitialized() const { return is_initialized; }
        
        // 서버 정보
        std::string getServerInfo() const;
        std::string getListenAddress() const;
        
        // 이벤트 루프 연동
        bool needsWriteEvent(int client_fd) const;
        bool needsReadEvent(int client_fd) const;
};
    