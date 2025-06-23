#include "EventLoop.hpp"

EventLoop::EventLoop()
{
    epoll_fd = -1;
    events = NULL;
    event_count = 0;
}

EventLoop::~EventLoop() {
    cleanup();
}

int EventLoop::initialize() {
    // epoll 인스턴스 생성
    epoll_fd = epoll_create(MAX_EVENTS);
    if (epoll_fd == -1) {
        return -1;  // epoll_create1 실패
    }
    
    // 이벤트 배열 할당
    events = new(std::nothrow) struct epoll_event[MAX_EVENTS];
    if (events == NULL) {
        close(epoll_fd);
        epoll_fd = -1;
        return -1;  // 메모리 할당 실패
    }
    
    return 0;  // 성공
}

void EventLoop::cleanup() {
    // EventData들 정리
    for (std::map<int, EventData*>::iterator it = fd_to_data.begin(); 
         it != fd_to_data.end(); ++it) {
        delete it->second;
    }
    fd_to_data.clear();
    
    // 이벤트 배열 해제
    if (events != NULL) {
        delete[] events;
        events = NULL;
    }
    
    // epoll 파일 디스크립터 닫기
    if (epoll_fd != -1) {
        close(epoll_fd);
        epoll_fd = -1;
    }
    
    event_count = 0;
}

int EventLoop::addSocket(int fd, uint32_t events, SocketType type, void* data) {
    if (epoll_fd == -1) {
        return -1;  // EventLoop가 초기화되지 않음
    }
    
    // 논블로킹 모드로 설정
    if (setNonBlocking(fd) != 0) {
        return -1;
    }
    
    // EventData 생성
    EventData* event_data = new(std::nothrow) EventData(fd, type, data);
    if (event_data == NULL) {
        return -1;  // 메모리 할당 실패
    }
    
    // epoll_event 구조체 설정
    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.ptr = event_data;  // EventData 포인터를 저장
    
    // epoll에 소켓 추가
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        delete event_data;
        return -1;  // epoll_ctl 실패
    }
    
    // 매핑 테이블에 추가
    fd_to_data[fd] = event_data;
    
    return 0;  // 성공
}

int EventLoop::modifySocket(int fd, uint32_t events) {
    if (epoll_fd == -1) {
        return -1;
    }
    
    // 기존 EventData 찾기
    std::map<int, EventData*>::iterator it = fd_to_data.find(fd);
    if (it == fd_to_data.end()) {
        return -1;  // 해당 fd가 등록되지 않음
    }
    
    // epoll_event 구조체 설정
    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.ptr = it->second;  // 기존 EventData 사용
    
    // epoll에서 소켓 수정
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        return -1;  // epoll_ctl 실패
    }
    
    return 0;  // 성공
}

int EventLoop::removeSocket(int fd) {
    if (epoll_fd == -1) {
        return -1;
    }
    
    // epoll에서 소켓 제거
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        // 이미 닫힌 소켓의 경우 EBADF 에러가 발생할 수 있음
        if (errno != EBADF) {
            return -1;
        }
    }
    
    // 매핑 테이블에서 제거
    std::map<int, EventData*>::iterator it = fd_to_data.find(fd);
    if (it != fd_to_data.end()) {
        delete it->second;
        fd_to_data.erase(it);
    }
    
    return 0;  // 성공
}

int EventLoop::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        return -1;
    }
    
    return 0;
}

int EventLoop::waitForEvents() {
    if (epoll_fd == -1 || events == NULL) {
        return -1;
    }
    
    // epoll_wait로 이벤트 대기
    event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, EPOLL_TIMEOUT);
    
    if (event_count == -1) {
        // EINTR은 시그널에 의한 중단이므로 정상적인 상황
        if (errno == EINTR) {
            event_count = 0;
            return 0;
        }
        return -1;  // 실제 에러
    }
    
    return event_count;  // 발생한 이벤트 수 반환
}

struct epoll_event& EventLoop::getEvent(int index) {
    // 범위 체크는 호출자가 해야 함
    return events[index];
}

EventData* EventLoop::getEventData(int index) {
    if (index < 0 || index >= event_count) {
        return NULL;
    }
    
    return static_cast<EventData*>(events[index].data.ptr);
}

std::string EventLoop::eventToString(uint32_t events) {
    std::string result;
    
    if (events & EPOLLIN)    result += "EPOLLIN ";
    if (events & EPOLLOUT)   result += "EPOLLOUT ";
    if (events & EPOLLERR)   result += "EPOLLERR ";
    if (events & EPOLLHUP)   result += "EPOLLHUP ";
    if (events & EPOLLRDHUP) result += "EPOLLRDHUP ";
    if (events & EPOLLET)    result += "EPOLLET ";
    if (events & EPOLLONESHOT) result += "EPOLLONESHOT ";
    
    return result.empty() ? "NONE" : result;
}