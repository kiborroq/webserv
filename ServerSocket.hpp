#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/socket.h>
# include <arpa/inet.h>
# include <string>
# include <cstring>
# include <fcntl.h>
# include <unistd.h>
# include <map>

# include "Config.hpp"
# include "ClientSocket.hpp"

# define BACKLOG 20

class ClientSocket;

class ServerSocket
{
	private:
		typedef std::map<int, ClientSocket> client_sock_map;

	private:
		server_context const* sc;
		int sock_fd;
		int optval;
		struct sockaddr_in addr;
		std::string socket_info;
		client_sock_map client_socks;

	public:
		explicit ServerSocket(server_context const& sc);
		ServerSocket(ServerSocket const& s);
		~ServerSocket(void);

		std::string const& getSocketInfo(void) const;
		int getSocketFD(void) const;
		server_context const& getServerContext(void) const;
		ClientSocket const* getClientSocket(int fd) const;

		int acceptClient(void);
		void removeClient(int fd);
		std::pair<bool, ClientSocket *> findClient(int fd);

	private:
		void setSocketAddr(void);
		void createAdjustSocket(void);
		void bindListenSocket(void);
};

#endif //SOCKET_HPP
