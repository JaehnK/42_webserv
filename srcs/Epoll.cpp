#include "Epoll.hpp"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <errno.h>

Epoll::Epoll(int maxEvents) : _maxEvents(maxEvents)
{
	_epfd = epoll_create(1);
	if (_epfd == -1)
	{
		std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
		throw std::runtime_error("Epoll creation failed");
	}
}

Epoll::~Epoll()
{
	if (close(_epfd) == -1)
	{
		std::cerr << "Failed to close epoll file descriptor: " << strerror(errno) << std::endl;
	}
}

int	Epoll::wait(epoll_event *events)
{
	int eventCount = epoll_wait(_epfd, events, _maxEvents, -1);
	if (eventCount == -1)
	{
		if (errno == EINTR)
		{
			std::cerr << "Epoll wait interrupted: " << strerror(errno) << std::endl;
			return 0;
		}
		else
		{
			std::cerr << "Epoll wait error: " << strerror(errno) << std::endl;
			return -1;
		}
	}
	return eventCount;
}

void	Epoll::add(int fd, uint32_t events)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = events;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		std::cerr << "Failed to add file descriptor to epoll: " << strerror(errno) << std::endl;
		throw std::runtime_error("Epoll add failed");
	}
}

void	Epoll::modify(int fd, uint32_t events)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = events;
	if (epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &event) == -1)
	{
		std::cerr << "Failed to modify file descriptor in epoll: " << strerror(errno) << std::endl;
		throw std::runtime_error("Epoll modify failed");
	}
}

void	Epoll::remove(int fd)
{
	if (epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
	{
		std::cerr << "Failed to remove file descriptor from epoll: " << strerror(errno) << std::endl;
		throw std::runtime_error("Epoll remove failed");
	}
}