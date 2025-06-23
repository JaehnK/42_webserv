#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <map>
#include <string>
#include <cstring>

enum SocketType {
    SOCKET_LISTEN,      // 서버 리슨 소켓
    SOCKET_CLIENT       // 클라이언트 연결 소켓
};

struct EventData {
    int fd;
    SocketType type;
    void* ptr;          // ServerConfig* 또는 ClientConnection*를 가리킴
    
    EventData(int socket_fd, SocketType socket_type, void* data_ptr = NULL) 
        : fd(socket_fd), type(socket_type), ptr(data_ptr) {}
};

class   EventLoop
{
    private:
        int                         epoll_fd;
        struct  epoll_event*        events;
        int                         event_count;
        std::map<int, EventData*>    fd_to_data;

        static const int MAX_EVENTS = 1024;
        static const int EPOLL_TIMEOUT = 1000;

    public:
        EventLoop();
        ~EventLoop();
        
        // 초기화
        int initialize();
        void cleanup();
        
        // 소켓 관리
        int addSocket(int fd, uint32_t events, SocketType type, void* data = NULL);
        int modifySocket(int fd, uint32_t events);
        int removeSocket(int fd);
        
        // 논블로킹 모드 설정
        int setNonBlocking(int fd);
        
        // 이벤트 대기 및 처리
        int waitForEvents();
        int getEventCount() const { return event_count; }
        struct epoll_event& getEvent(int index);
        EventData* getEventData(int index);
        
        // 유틸리티
        bool isValid() const { return epoll_fd != -1; }
        static std::string eventToString(uint32_t events);
};