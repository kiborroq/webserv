#include "WebServer.hpp"

WebServer::WebServer(Config::servers_map const& s)
	: serv_socks(), poll_fds()
{
	Config::servers_map::const_iterator it;
	for (it = s.begin(); it != s.end(); ++it)
	{
		ServerSocket tmp((*it).second);
		try
		{
			serv_socks.push_back(tmp);
			poll_fds.addFD(tmp.getSocketFD(), POLLIN);
			Logger(tmp.getSocketInfo() + " was created, adjusted and bound", OK);
		}
		catch (char const* e)
		{
			std::cout << e << std::endl;
			Logger(tmp.getSocketInfo() + " wasn't created, adjusted and bound because " + std::string(e), OK);
		}
	}
}

WebServer::~WebServer(void)
{ }

ClientSocket * WebServer::findClientSocket(int fd)
{
	std::pair<bool, ClientSocket *> p;

	for (size_t i = 0; i < serv_socks.size(); ++i)
	{
		p = serv_socks[i].findClient(fd);
		if (p.first)
			return p.second;
	}
	throw "error, where is client?";
}

void WebServer::cleanAllFromFD(size_t i)
{
	int fd = poll_fds[i].fd;

	if (i < serv_socks.size())
		serv_socks.erase(serv_socks.begin() + i);
	else
	{
		ServerSocket & for_client_remove = const_cast<ServerSocket &>(findClientSocket(poll_fds[i].fd)->getParentSocket());
		for_client_remove.removeClient(fd);
	}
	poll_fds.removeFD(i);
}

void WebServer::mainLoop(void)
{
	int num_fd;

	while (21)
	{
		if ((num_fd = poll( poll_fds.getFDs(), poll_fds.size(), -1 )) < 0)
			throw "poll error";
		for (size_t i = 0; num_fd > 0 && i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].revents != 0)
				num_fd--;
			try
			{
				if (poll_fds[i].revents & POLLIN)
				{
					if (i < serv_socks.size())
					{
						ServerSocket & for_accept = serv_socks[i];
						int new_client = for_accept.acceptClient();

						if (new_client < 0)
							throw for_accept.getSocketInfo() + " did not accept new client, because error occurred";
						poll_fds.addFD(new_client, POLLIN | POLLOUT);
						Logger(for_accept.getSocketInfo() + " accepted new " + for_accept.getClientSocket(new_client)->getSocketInfo(), OK);
					}
					else
					{
						ClientSocket * for_read = findClientSocket(poll_fds[i].fd);
						std::string socket_info = for_read->getSocketInfo();

						if (for_read->receiveRequest() < 0)
						{
							cleanAllFromFD(i--);
							throw socket_info + " did not receive request because error occurred. This client socket was deleted";
						}
						Logger(socket_info + " received request", OK);
					}
					poll_fds[i].revents = 0;
				}
				else if (poll_fds[i].revents & POLLOUT)
				{
					ClientSocket * for_write = findClientSocket(poll_fds[i].fd);
					std::string socket_info = for_write->getSocketInfo();

					if (for_write->readyForSending())
					{
						for_write->prepareResponse();
						int sent = for_write->sendResponse();
						cleanAllFromFD(i--);
						if (sent < 0)
							throw socket_info + " did not send response because error occurred. This client socket was deleted";
						Logger( socket_info+ " sent response. This client socket was deleted", OK);
					}
					else
						poll_fds[i].revents = 0;
				}
				else if (poll_fds[i].revents != 0)
				{
					if (poll_fds[i].revents & POLLNVAL)
						std::cout << "POLLNVAL" << std::endl;
					else if (poll_fds[i].revents & POLLHUP)
						std::cout << "POLLHUP" << std::endl;
					else if (poll_fds[i].revents & POLLERR)
						std::cout << "POLLERR" << std::endl;
					cleanAllFromFD(i--);
				}
			}
			catch (char const* message)
			{
				Logger(message, KO);
			}
		}
		if (poll_fds.size() == 0)
			break ;
	}
}
