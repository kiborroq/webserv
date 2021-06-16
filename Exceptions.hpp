#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <string>
# include <stdexcept>

class ProgramException : public std::exception
{
	private:
		std::string message;

	public:
		explicit ProgramException(std::string const& message) throw();
		ProgramException(ProgramException const& cpe) throw();
		ProgramException & operator=(ProgramException const& cpe) throw();

		virtual const char * what(void) const throw();
		virtual ~ProgramException(void) throw();
};

class ConfigException : public ProgramException
{
	public:
		ConfigException(std::string const& message) : ProgramException(message) { }
};

class ThrowMessage : public std::exception
{
	private:
		std::string message;

	public:
		explicit ThrowMessage(std::string const& message) throw();
		ThrowMessage(ThrowMessage const& cpe) throw();
		ThrowMessage & operator=(ThrowMessage const& cpe) throw();

		virtual const char * what(void) const throw();
		virtual ~ThrowMessage(void) throw();
};

#endif //EXCEPTIONS_HPP
