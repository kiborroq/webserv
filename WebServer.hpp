#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# define TIMEOUT 60000

# include <vector>
# include <sys/poll.h>
# include "Exceptions.hpp"
# include "Logger.hpp"
# include "ServerSocket.hpp"
# include "PollFDs.hpp"

class WebServer
{
	public:
		typedef std::vector<ServerSocket> serv_sock_vector;

	private:
		serv_sock_vector serv_socks;
		PollFDs poll_fds;

	public:
		WebServer(Config::servers_map const& s);
		~WebServer(void);

		void mainLoop(void);

	private:
		ClientSocket * findClientSocket(int fd);
		void cleanAllFromFD(size_t i);
		std::string getSocketInfo(size_t i);
		std::string translatePollError(short revents);
};

#endif //WEBSERVER_HPP
