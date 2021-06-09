#ifndef SOCKET_HPP
# define SOCKET_HPP

//# include <sys/socket.h>
//# include <netdb.h>
# include <string>
# include <cstring>

#include <ws2tcpip.h> //for windows
#include <wspiapi.h> //for windows

# include "utils.hpp"
# include "Config.hpp"

# define BACKLOG 100;

class Socket
{
	private:
		server_context const& sc;
		struct addrinfo *server_info;
		int fdsock;
		bool is_active;

	public:
		Socket(server_context const& sc);
		Socket(Socket const& s);
		~Socket(void);

		Socket & operator=(Socket const& s);

	private:
		void initServerInfo(void);
		void createSocket(void);
};

#endif //SOCKET_HPP
