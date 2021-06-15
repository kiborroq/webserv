#include "parsing_utils.hpp"

bool validMaxBodySize(std::string const& size)
{
	str_const_it it = size.begin();

	for ( ; it < size.end() - 1; it++)
	{
		if (!isdigit(*it))
			return false;
	}
	if (!isSizeLetter(*it) && !isdigit(*it))
		return false;
	return true;
}

bool validMethod(std::string const& method)
{
	size_t const count = 5;
	std::string methods[count] = { "GET", "POST", "DELETE", "PUT", "HEAD" };

	for (size_t i = 0; i < count; i++)
	{
		if (method == methods[i])
			return true;
	}
	return false;
}

bool validDirective(std::string const& value)
{
	if (iscontained(value, ';') ||
		iscontained(value, '{') ||
		iscontained(value, '}') )
		return false;
	return true;
}

bool validHost(std::string & host)
{
	if (host == "")
		return false;
	if (host == "localhost")
		return true;

	int num_parts = 0;
	std::string part;
	str_it curr = host.begin();
	str_it end = host.end();

	while (curr != end && num_parts <= 4)
	{
		part = getWord(curr, end, '.');
		if ((curr != end && *curr != '.') ||
			part.size() > 3 || part.size() < 1 ||
			(part.size() == 3 && part[0] == '0') ||
			(part.size() == 2 && part[0] == '0') ||
			!isonlydigits(part, part.size()) ||
			ato_all<int>(part) < 0 || ato_all<int>(part) > 255)
			return false;
		if (curr != end)
			curr++;
		num_parts++;
	}
	if (num_parts != 4)
		return false;
	return true;
}

bool validPort(std::string const& port_s)
{
	size_t port_st = ato_all<size_t>(port_s);

	if (!isonlydigits(port_s, port_s.size()) ||
		port_st < 0 || port_st > getMaxValue<unsigned short>())
		return false;
	return true;
}

bool validHttpCode(std::string const& code_s, int lower_bound, int upper_bound)
{
	if (code_s.size() == 0 || !isonlydigits(code_s, code_s.size()))
		return false;
	int code_i = ato_all<int>(code_s);
	if (code_i < lower_bound || code_i > upper_bound)
		return false;
	return true;
}

bool validCgiExtension(std::string const& extension)
{
	size_t const count = 3;
	std::string extensions[count] = { "py", "php", "html" };

	for (size_t i = 0; i < count; i++)
	{
		if (extension == extensions[i])
			return true;
	}
	return false;
}

size_t tobytes(std::string const& size)
{
	size_t bytes;
	size_t uint64_max = getMaxValue<unsigned int>();

	char sizeLetter = *(size.end() - 1);
	bytes = ato_all<size_t>(std::string(size.begin(), size.end() - 1));
	if (sizeLetter == 'k' || sizeLetter == 'K')
		bytes = bytes <= uint64_max / 1024 ?
				bytes * 1024 : uint64_max;
	else if (sizeLetter == 'm' || sizeLetter == 'M')
		bytes = bytes <= uint64_max / (1024 * 1024) ?
				bytes * (1024 * 1024) : uint64_max;
	else if (sizeLetter == 'g' || sizeLetter == 'G')
		bytes = bytes <= uint64_max / (1024 * 1024 * 1024) ?
				bytes * (1024 * 1024 * 1024) : uint64_max;
	return  bytes;
}

std::string getFileContent(char const* config_path)
{
	std::string line;
	std::string content;

	std::ifstream fin(config_path);

	if (fin.fail())
		throw "Config file error!";

	while (!fin.fail() && !fin.eof())
	{
		getline(fin, line);
		content += line;
	}
	return content;
}

std::string charToString(char c)
{
	std::string s;
	s.push_back(c);
	return  s;
}

std::string getWord(str_it & curr_pos, str_it const& end, char delimiter)
{
	str_it start_word;

	while (curr_pos != end && isspace(*curr_pos))
		curr_pos++;

	if (curr_pos == end)
		return "";
	else if (*curr_pos == delimiter)
	{
		curr_pos++;
		return charToString(delimiter);
	}

	start_word = curr_pos;
	while (curr_pos != end && !isspace(*curr_pos) && *curr_pos != delimiter)
		curr_pos++;
	return std::string(start_word, curr_pos);
}

bool isonlydigits(std::string const& s, size_t count)
{
	for (size_t i = 0; i < count && i < s.size(); i++)
	{
		if (!isdigit(s[i]))
			return false;
	}
	return true;
}

bool iscontained(std::string const& s, char c)
{
	return s.find(c) != std::string::npos;
}

bool isSizeLetter(char c)
{
	if (c != 'k' && c != 'm' && c != 'g' &&
		c != 'K' && c != 'M' && c != 'G' )
		return false;
	return true;
}

std::string ft_inet_ntoa(struct in_addr const& addr)
{
	std::stringstream ip;

	unsigned char const *bytes = reinterpret_cast<unsigned char const*>(&addr);
	for (int cur_bytes = 0; cur_bytes < 4; cur_bytes++)
	{
		ip << static_cast<int>(bytes[cur_bytes]);
		if (cur_bytes != 3)
			ip << '.';
	}
	return (ip.str());
}
