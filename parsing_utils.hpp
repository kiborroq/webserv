#ifndef PARSING_UTILS_HPP
# define PARSING_UTILS_HPP

# include <sstream>
# include <fstream>
# include <limits>

#include "Exceptions.hpp"

template <typename Size>
size_t getMaxValue(void)
{
	return std::numeric_limits<Size>::max();
}

template <typename T>
T ato_all(std::string const& s)
{
	std::stringstream stream;
	T num;

	stream << s;
	stream >> num;

	return num;
}

template <typename T>
std::string all_toa(T num)
{
	std::stringstream stream;
	stream << num;
	return stream.str();
}

typedef std::string::iterator str_it;
typedef std::string::const_iterator str_const_it;

std::string getFileContent(char const* config_path, bool & iserror);
std::string getWord(str_it & curr_pos, str_it const& end, char delimiter = ' ');

bool validHttpCode(std::string const& code, int lower_bound = 0, int upper_bound = 999);
bool validPort(std::string const& port);
bool validHost(std::string & host);
bool validMaxBodySize(std::string const& size);
bool validCgiExtension(std::string const& extension);
bool validMethod(std::string const& method);
bool validDirective(std::string const& value);

size_t tobytes(std::string const& size);
bool isonlydigits(std::string const& s, size_t count);
bool iscontained(std::string const& s, char c);

std::string charToString(char c);
bool isSizeLetter(char c);

std::string ft_inet_ntoa(struct in_addr const& addr);

#endif // PARSING_UTILS_HPP
