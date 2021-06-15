#ifndef POLLFDS_HPP
# define POLLFDS_HPP

# include <vector>
# include <algorithm>
# include <sys/poll.h>
# include <unistd.h>

class PollFDs : public std::vector<pollfd>
{
	public:
		PollFDs(void);
		~PollFDs(void);

		pollfd *getFDs(void);
		void addFD(int fd, short events);
		void removeFD(int i);
};

#endif //POLLFDS_HPP
