#include "Socket.hpp"

Socket::Socket(server_context const& sc)
	: sc(sc), fdsock(), optval(), is_active(false)
{
	struct addrinfo *server_info;

	server_info = getServerInfo();
	createSocket(server_info);
	adjustSocket();
	bindListenSocket(server_info);
	freeaddrinfo(server_info);
}

Socket::Socket(Socket const& s)
	: sc(s.sc), fdsock(s.fdsock), optval(s.optval), is_active(s.is_active)
{ }

struct addrinfo *Socket::getServerInfo(void)
{
	struct addrinfo *server_info;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(sc.host.c_str(), all_toa(sc.port).c_str(), &hints, &server_info) == -1)
		throw "socket";
	return server_info;
}

void Socket::createSocket(struct addrinfo *server_info)
{
    fdsock = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (fdsock == -1)
        throw strerror(fdsock);
}

void Socket::adjustSocket(void)
{
	if (setsockopt(fdsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
		throw "socket";

	if (fcntl(fdsock, F_SETFL, O_NONBLOCK) == -1)
		throw "socket";
}

void Socket::bindListenSocket(struct addrinfo *server_info)
{
	struct addrinfo *tmp;
	for (tmp = server_info; tmp != NULL; tmp = tmp->ai_next)
	{
		if (bind(fdsock, tmp->ai_addr, tmp->ai_addrlen) == 0 &&
			listen(fdsock, BACKLOG) == 0)
			break;
	}
	if (tmp == NULL)
		throw "socket";
	is_active = true;
}

int Socket::getFdSock(void)
{ return fdsock; }

server_context const& Socket::getServerContext(void)
{ return sc; }
