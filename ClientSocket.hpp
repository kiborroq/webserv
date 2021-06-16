#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>
# include "ServerSocket.hpp"

class ServerSocket;

class ClientSocket
{
	private:
		int sock_fd;
		ServerSocket const* parent;
		sockaddr_in addr;
		std::string socket_info;
		std::string request;
		bool ready_to_send;
		std::string response;

	public:
		ClientSocket(int fd, sockaddr & saddr, ServerSocket const& s);
		ClientSocket(ClientSocket const& c);
		~ClientSocket(void);

		ClientSocket & operator=(ClientSocket const& c);

		int receiveRequest(void);
		void prepareResponse(void);
		int sendResponse(void);

		bool readyForSending(void) const;

		std::string getSocketInfo(void) const;
		ServerSocket const& getParentSocket(void) const;
		int getSocketFD(void) const;
};

#endif //CLIENT_HPP
