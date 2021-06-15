#include "PollFDs.hpp"

PollFDs::PollFDs(void) : std::vector<pollfd>()
{ }

PollFDs::~PollFDs(void)
{ }

pollfd * PollFDs::getFDs()
{
	return this->size() == 0 ? NULL : &this->operator[](0);
}

void PollFDs::addFD(int fd, short events)
{
	pollfd tmp;
	tmp.fd = fd;
	tmp.events = events;
	tmp.revents = 0;

	this->push_back(tmp);
}

void PollFDs::removeFD(int i)
{
	close(this->operator[](i).fd);
	this->erase(begin() + i);
}
