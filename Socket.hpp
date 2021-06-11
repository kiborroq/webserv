#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <string>
# include <cstring>
# include <fcntl.h>

# include "parsing_utils.hpp"
# include "Config.hpp"

# define BACKLOG 100

class Socket
{
	private:
		server_context const& sc;
		int fdsock;
		int optval;
		bool is_active;

	public:
		Socket(server_context const& sc);
		Socket(Socket const& s);
		~Socket(void);

		int getFdSock(void);
		server_context const& getServerContext(void);

	private:
		struct addrinfo *getServerInfo(void);
		void createSocket(struct addrinfo *server_info);
		void adjustSocket(void);
		void bindListenSocket(struct addrinfo *server_info);
};

#endif //SOCKET_HPP
