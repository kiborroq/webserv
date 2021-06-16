#include "Exceptions.hpp"

ProgramException::ProgramException(const std::string &message) throw()
	: message(message)
{ }

ProgramException::ProgramException(const ProgramException &cpe) throw()
	: message(cpe.message)
{ }

ProgramException::~ProgramException() throw()
{ }

ProgramException & ProgramException::operator=(const ProgramException &cpe) throw()
{ this->message = cpe.message; return *this;}

const char * ProgramException::what(void) const throw()
{ return message.c_str(); }


ThrowMessage::ThrowMessage(const std::string &message) throw()
		: message(message)
{ }

ThrowMessage::ThrowMessage(const ThrowMessage &cpe) throw()
		: message(cpe.message)
{ }

ThrowMessage::~ThrowMessage() throw()
{ }

ThrowMessage & ThrowMessage::operator=(const ThrowMessage &cpe) throw()
{ this->message = cpe.message; return *this; }

const char * ThrowMessage::what(void) const throw()
{ return message.c_str(); }
