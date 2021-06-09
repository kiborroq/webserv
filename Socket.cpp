#include "Socket.hpp"

Socket::Socket(server_context const& sc)
	: sc(sc)
{
	initAddrInfo();
	createSocket();
}

void Socket::initServerInfo(void)
{
	int status;
    struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (((status = getaddrinfo(sc.host.c_str(), all_toa(sc.port).c_str(), &hints, &server_info))) != 0)
		throw gai_strerror(status);
}

void Socket::createSocket(void)
{
    fdsock = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (fdsock == -1)
        throw strerror(fdsock);
}


