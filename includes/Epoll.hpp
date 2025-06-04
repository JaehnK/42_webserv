#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <sys/epoll.h>

class Epoll
{
	private:
		int	_epfd;
		int	_maxEvents;

	public:
		Epoll(int maxEvents = 64);
		~Epoll();

		int		wait(epoll_event *events);
		void	add(int fd, uint32_t events);
		void	modify(int fd, uint32_t events);
		void	remove(int fd);
};

#endif /* EPOLL_HPP */