#ifndef CLIENT_HPP
# define CLIENT_HPP

# define READSIZE 100000
# define MAX_NOT_ACTIVE_TIME 10

# include <sys/socket.h>
# include "CHttpRequest.hpp"
# include "ServerSocket.hpp"

class ServerSocket;

class ClientSocket
{
	private:
		long last_active_time;
		int sock_fd;
		ServerSocket const* parent;
		sockaddr_in addr;
		std::string socket_info;
		CHttpRequest httpTreatment;
		std::string request;
		int status;
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

		bool isactive();
		void reset_last_active_time();
};

#endif //CLIENT_HPP
