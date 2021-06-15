#include "ServerSocket.hpp"

ServerSocket::ServerSocket(server_context const& sc)
	: sc(&sc), sock_fd(), optval(1)
{
	socket_info = "server socket " + sc.host + ":" + all_toa(sc.port);

	setSocketAddr();
	createAdjustSocket();
	bindListenSocket();
}

ServerSocket::ServerSocket(ServerSocket const& s)
	: sc(s.sc), sock_fd(s.sock_fd), optval(s.optval), addr(s.addr),
	  socket_info(s.socket_info), client_socks(s.client_socks)
{ }

ServerSocket::~ServerSocket()
{ }

void ServerSocket::setSocketAddr(void)
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (sc->host != "localhost")
		addr.sin_addr.s_addr = inet_addr(sc->host.c_str());
	else
		addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(sc->port);
}

void ServerSocket::createAdjustSocket(void)
{
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
		throw "socket error";

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
		throw "setopt error";

	if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) == -1)
		throw "fctnl server error";
}

void ServerSocket::bindListenSocket(void)
{
	if (bind(sock_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
		throw "bind error";

	if (listen(sock_fd, BACKLOG) != 0)
		throw "listen error";
}

int ServerSocket::acceptClient(void)
{
	int new_client_fd;
	sockaddr addr_client;
	socklen_t size = sizeof(addr_client);

	if ((new_client_fd = accept(sock_fd, &addr_client, &size)) < 0)
		return -1;
	if (fcntl(new_client_fd, F_SETFL, O_NONBLOCK) == -1)
		throw -1;

	client_socks.insert(std::make_pair(new_client_fd, ClientSocket(new_client_fd, addr_client, *this) ) );
	return new_client_fd;
}

void ServerSocket::removeClient(int fd)
{ client_socks.erase(fd); }

std::pair<bool, ClientSocket *> ServerSocket::findClient(int fd)
{
	client_sock_map::iterator it = client_socks.find(fd);
	if (it != client_socks.end())
		return std::make_pair( true, &it->second );

	ClientSocket *tmp = NULL;
	return std::make_pair(false, tmp);
}

std::string const& ServerSocket::getSocketInfo(void) const
{ return socket_info; }

int ServerSocket::getSocketFD(void) const
{ return sock_fd; }

server_context const& ServerSocket::getServerContext(void) const
{ return *sc; }

ClientSocket const* ServerSocket::getClientSocket(int fd) const
{
	client_sock_map::const_iterator it = client_socks.find(fd);
	if (it != client_socks.end())
		return &(*it).second;
	return NULL;
}
