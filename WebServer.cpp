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
			Logger(tmp.getSocketInfo() + " was created, adjusted and bound.", OK);
		}
		catch (ThrowMessage const& tm)
		{
			Logger(tmp.getSocketInfo() + " wasn't created: " + tm.what(), KO);
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
	return NULL;
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

std::string WebServer::getSocketInfo(size_t i)
{
	if (i < serv_socks.size())
		return serv_socks[i].getSocketInfo();
	else
		return findClientSocket(poll_fds[i].fd)->getSocketInfo();
}

std::string WebServer::translatePollError(short revents)
{
	if (revents & POLLNVAL)
		return "Socket was not created.";
	else if (revents & POLLHUP)
		return "Socket broken connection.";
	else
		return "Error occurred with socket.";
}

void WebServer::mainLoop(void)
{
	std::string socket_info;

	while (21)
	{
		if (poll(poll_fds.getFDs(), poll_fds.size(), 5000) < 0)
			throw ProgramException(strerror(errno));
		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			socket_info = getSocketInfo(i);

			try
			{
				if (poll_fds[i].revents & POLLIN)
				{
					if (i < serv_socks.size())
					{
						ServerSocket & for_accept = serv_socks[i];
						int new_client = for_accept.acceptClient();

						if (new_client < 0)
							throw ThrowMessage(socket_info +
								" did not accept new client, because: " + strerror(errno));

						poll_fds.addFD(new_client, POLLIN | POLLOUT);
						Logger(socket_info + " accepted new " +
							for_accept.getClientSocket(new_client)->getSocketInfo() + ".", OK);
					}
					else
					{
						ClientSocket * for_read = findClientSocket(poll_fds[i].fd);

						int nbr = for_read->receiveRequest();
						if (nbr < 0)
						{
							cleanAllFromFD(i--);
							throw ThrowMessage(socket_info + " did not receive request, because: " +
								strerror(errno) + " This client socket was deleted.");
						}
						else if (nbr > 0)
						{
							if (for_read->readyForSending())
								Logger(socket_info + " received request.", OK);
							for_read->reset_last_active_time();
						}
						else if (nbr == 0)
						{
							if (!for_read->isactive())
							{
								cleanAllFromFD(i--);
								throw ThrowMessage(socket_info + " was deleted because: client isn't active already " +
												   all_toa(MAX_NOT_ACTIVE_TIME) + ".");
							}
						}
					}
					poll_fds[i].revents = 0;
				}
				else if (poll_fds[i].revents & POLLOUT)
				{
					ClientSocket * for_write = findClientSocket(poll_fds[i].fd);

					if (for_write->readyForSending())
					{
						for_write->prepareResponse();
						int nbr = for_write->sendResponse();
						if (nbr < 0)
						{
							cleanAllFromFD(i--);
							throw ThrowMessage(socket_info + " did not send response, because: " +
											   strerror(errno) + " This client socket was deleted.");
						}
						else if (nbr > 0)
							Logger(socket_info + " sent response.", OK);
						for_write->reset_last_active_time();
					}
					else
						poll_fds[i].revents = 0;
				}
				else if (poll_fds[i].revents == 0 && i >= serv_socks.size())
				{
					ClientSocket * client = findClientSocket(poll_fds[i].fd);

					if (!client->isactive())
					{
						cleanAllFromFD(i--);
						throw ThrowMessage(socket_info + " was deleted because: client isn't active already " +
											all_toa(MAX_NOT_ACTIVE_TIME) + ".");
					}
				}
				else if (poll_fds[i].revents != 0)
				{
					std::string error = translatePollError(poll_fds[i].revents);

					cleanAllFromFD(i--);
					throw ThrowMessage(socket_info + " was deleted because: " + error);
				}
			}
			catch (ThrowMessage const& tm)
			{
				Logger(tm.what(), KO);
			}
		}
		if (poll_fds.size() == 0)
			break ;
	}
}
