#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/socket.h>
# include <netdb.h>
# include <string>
# include <cstring>

# include "utils.hpp"
# include "Config.hpp"

class Socket
{
	private:
		server_context const& sc;
		struct addrinfo *info;
		struct addrinfo hints;
		int fdsock;

	public:
		Socket(server_context const& sc);
		Socket(Socket const& s);
		~Socket(void);

		Socket & operator=(Socket const& s);

	private:
		void initAddrInfo(void);
		void createSocket(void);
};

#endif //SOCKET_HPP
