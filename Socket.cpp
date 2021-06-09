#include "Socket.hpp"

Socket::Socket(server_context const& sc)
	: sc(sc)
{
	initAddrInfo();
}

void Socket::initAddrInfo()
{
	int status;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (((status = getaddrinfo(sc.host.c_str(), all_toa(sc.port).c_str(), &hints, &info))) != 0)
		throw strerror(status);
}

void Socket::createSocket()
{

}
