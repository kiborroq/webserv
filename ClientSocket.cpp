#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fd, sockaddr & saddr, ServerSocket const& s)
	: sock_fd(fd), parent(&s)
{
	sockaddr_in *tmp = reinterpret_cast<sockaddr_in *>(&saddr);
	addr = *tmp;

	socket_info =	"client socket " +
					ft_inet_ntoa(addr.sin_addr) + ":" +
					all_toa(ntohs(addr.sin_port));
}

ClientSocket::ClientSocket(const ClientSocket &c)
	: sock_fd(c.sock_fd), parent(c.parent), addr(c.addr), socket_info(c.socket_info),
	request(c.request), ready_to_send(c.ready_to_send), response(c.response)
{ }

ClientSocket::~ClientSocket(void)
{  }

ClientSocket & ClientSocket::operator=(ClientSocket const& c)
{
	this->sock_fd = c.sock_fd;
	this->parent = c.parent;
	this->addr = c.addr;
	this->request = c.request;
	this->ready_to_send = c.ready_to_send;
	this->response = c.response;
	return *this;
}

int ClientSocket::receiveRequest(void)
{
	char buf[BUFSIZ + 1];
	memset(buf, 0, BUFSIZ + 1);
	int received = recv(sock_fd, buf, BUFSIZ, 0);
	if (received > 0)
	{
		request.append(buf);
		ready_to_send = true;
	}
	return received;
}

void ClientSocket::prepareResponse(void)
{
	response = "HTTP/1.1 301 Moved Permanently\n"
			   "Location: http://www.google.com/\n"
			   "Content-Type: text/html; charset=UTF-8\n"
			   "Date: Tue, 15 Jun 2021 09:23:05 GMT\n"
			   "Expires: Thu, 15 Jul 2021 09:23:05 GMT\n"
			   "Cache-Control: public, max-age=2592000\n"
			   "Server: gws\n"
			   "Content-Length: 219\n"
			   "X-XSS-Protection: 0\n"
			   "X-Frame-Options: SAMEORIGIN\n\n"
			   "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"
			   "<TITLE>301 Moved</TITLE></HEAD><BODY>\n"
			   "<H1>301 Moved</H1>\n"
			   "The document has moved\n"
			   "<A HREF=\"http://www.google.com/\">here</A>.\n"
			   "</BODY></HTML>\n\n\n";
}

int ClientSocket::sendResponse(void)
{
	int res = send(3453, response.c_str(), response.size(), 0);
	ready_to_send = false;
	return res;
}

bool ClientSocket::readyForSending(void) const
{ return ready_to_send; }

std::string ClientSocket::getSocketInfo(void) const
{ return socket_info; }

ServerSocket const& ClientSocket::getParentSocket(void) const
{ return *parent; }

int ClientSocket::getSocketFD(void) const
{ return sock_fd; }
