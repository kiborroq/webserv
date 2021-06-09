/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiborroq <kiborroq@kiborroq.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/03 13:48:12 by kiborroq          #+#    #+#             */
/*   Updated: 2021/06/07 22:51:16 by kiborroq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(char const* config_path)
	: UINT64_MAX(std::numeric_limits<size_t>::max()),
	  UINT16_MAX(std::numeric_limits<unsigned short>::max()),
	  servers()
{
	std::string content = getFileContent(config_path);
	str_it curr = content.begin();
	str_it end = content.end();
	std::string word;

	while (curr != end)
	{
		word = getWord(curr, end, '{');
		if (word == "server")
			setServer(curr, end, servers);
		else
			throw "Config 1!";
	}
}

Config::~Config(void) { }

bool Config::validHost(std::string & host)
{
	if (host == "localhost" || host == "*")
		return true;
	if (host == "")
		return false;

	int num_parts = 0;
	std::string part;
	str_it curr = host.begin();
	str_it end = host.end();

	while (curr != end && num_parts <= 4)
	{
		part = getWord(curr, end, '.');
		if (curr == end || *curr != '.' ||
			part.size() > 3 || part.size() < 1 ||
			(part.size() == 3 && part[0] == '0') ||
			(part.size() == 2 && part[0] == '0') ||
			!isonlydigits(part, part.size()) ||
			ato_all<int>(part) < 0 || ato_all<int>(part) > 255)
			return false;
		curr++;
		num_parts++;
	}
	if (num_parts != 4)
		return false;
	return true;
}

bool Config::validPort(std::string const& port_s)
{
	size_t port_st = ato_all<size_t>(port_s);

	if (!isonlydigits(port_s, port_s.size()) ||
		port_st < 0 || port_st > UINT16_MAX)
		return false;
	return true;
}

bool does_contain(std::string const& s, char c)
{
	return s.find(c) != std::string::npos;
}

void Config::setHostPort(str_it & curr_pos, str_it const& end, server_context & s)
{
	std::string host_port = getWord(curr_pos, end, ';');
	if (host_port == ";" || getWord(curr_pos, end, ';') != ";")
		throw "Config 2";

	str_it curr_hp = host_port.begin();
	str_it end_hp = host_port.end();

	if (does_contain(host_port, ':'))
	{
		s.host = getWord(curr_hp, end_hp, ':');
		if (!validHost(s.host) || ++curr_hp == end_hp ||
			!validPort(std::string(curr_hp, end_hp)))
			throw "Config 3";
		s.port = ato_all<size_t>(getWord(curr_hp, end_hp));
	}
	else if (validHost(host_port))
		s.host = host_port;
	else if (validPort(host_port))
		s.port = ato_all<size_t>(host_port);
	else
		throw "Config 4";
}

bool validDirective(std::string const& value)
{
	if (does_contain(value, ';') ||
		does_contain(value, '{') ||
		does_contain(value, '}') )
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

void Config::setPath(str_it & curr_pos, str_it const& end, std::string & path)
{
	path = getWord(curr_pos, end, ';');
	if (!validDirective(path) || *curr_pos != ';')
		throw "Config file error!";
	curr_pos++;
}

void Config::setArray(str_it & curr_pos, str_it const& end, std::list<std::string> & l, bool (*valid)(std::string const& s))
{
	std::string elem = getWord(curr_pos, end, ';');
	if (elem == ";")
		throw "Config file error!";

	while (curr_pos != end && elem != ";" )
	{
		if (!valid(elem))
			throw "Config file error!";
		l.push_back(elem);
		elem = getWord(curr_pos, end, ';');
	}
	if (curr_pos == end)
		throw "Config file error!";
}

bool isSizeLetter(char c)
{
	if (c != 'k' && c != 'm' && c != 'g' &&
		c != 'K' && c != 'M' && c != 'G' )
		return false;
	return true;
}

bool Config::validMaxBodySize(std::string const& size)
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

size_t Config::toBytes(std::string const& size)
{
	size_t bytes;
	char sizeLetter = *(size.end() - 1);
	bytes = ato_all<size_t>(std::string(size.begin(), size.end() - 1));
	if (sizeLetter == 'k' || sizeLetter == 'K')
		bytes = bytes <= UINT64_MAX / 1024 ?
				bytes * 1024 : UINT64_MAX;
	else if (sizeLetter == 'm' || sizeLetter == 'M')
		bytes = bytes <= UINT64_MAX / (1024 * 1024) ?
			   bytes * (1024 * 1024) : UINT64_MAX;
	else if (sizeLetter == 'g' || sizeLetter == 'G')
		bytes = bytes <= UINT64_MAX / (1024 * 1024 * 1024) ?
			   bytes * (1024 * 1024 * 1024) : UINT64_MAX;
	return  bytes;
}

void Config::setMaxBodySize(str_it & curr_pos, str_it const& end, size_t & client_max_body_size)
{
	std::string size = getWord(curr_pos, end, ';');
	if (!validMaxBodySize(size) || getWord(curr_pos, end, ';') != ";")
		throw "Config file error!";

	if (isalpha(*(size.end() - 1)))
		client_max_body_size = toBytes(size);
	else
		client_max_body_size = ato_all<size_t>(size);
}

bool Config::isonlydigits(std::string const& s, size_t count)
{
	for (size_t i = 0; i < count && i < s.size(); i++)
	{
		if (!isdigit(s[i]))
			return false;
	}
	return true;
}

bool Config::validHttpCode(std::string const& code_s, int lower_bound, int upper_bound)
{
	if (code_s.size() == 0 || !isonlydigits(code_s, code_s.size()))
		return false;
	int code_i = ato_all<int>(code_s);
	if (code_i < lower_bound || code_i > upper_bound)
		return false;
	return true;
}

void Config::setErrorPage(str_it & curr_pos, str_it const& end, errorpages_map & error_pages)
{
	std::list<int> save_code;

	std::string word = getWord(curr_pos, end, ';');
	while (validHttpCode(word, 400, 599))
	{
		save_code.push_back(ato_all<int>(word));
		word = getWord(curr_pos, end, ';');
	}
	if (validDirective(word) && (getWord(curr_pos, end, ';')) == ";")
	{
		for (std::list<int>::iterator it = save_code.begin() ; it != save_code.end() ; ++it)
			error_pages.insert(std::make_pair(*it, word));
	}
	else
		throw "Config file error!";
}

void Config::setEnable(str_it & curr_pos, str_it const& end, bool & enable)
{
	std::string tmp = getWord(curr_pos, end, ';');
	if ((tmp != "on" && tmp != "off") || getWord(curr_pos, end, ';') != ";")
		throw "Config file error!";
	enable = tmp == "on" ? true : false;
}

void Config::setRedirect(str_it & curr_pos, str_it const& end, std::pair<int, std::string> & redirect)
{
	std::string code = getWord(curr_pos, end);
	std::string url = getWord(curr_pos, end, ';');
	if (!validHttpCode(code, 300, 399) || !validDirective(url) || (getWord(curr_pos, end, ';')) != ";")
		throw  "Config file error!";
	redirect.first = ato_all<int>(code);
	redirect.second = url;
}

bool Config::validCgiExtension(std::string const& extension)
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

void Config::setCgiExtension(str_it & curr_pos, str_it const& end, std::string & extension)
{
	extension = getWord(curr_pos, end, ';');
	if (!validCgiExtension(extension) || getWord(curr_pos, end, ';') != ";")
		throw "Config file error!";
}

void Config::initLocation(location_context & l, server_context const& src)
{
	l.location_name = "/";
	l.root = src.root;
	l.index = src.index;
	l.client_max_body_size = src.client_max_body_size;
	l.autoindex = src.autoindex;
	l.default_file_path = "default";
	l.cgi_path = "cgi";
	l.uploading_path = "uploading";
}

void Config::setLocation(str_it & curr_pos, str_it const& end, locations_map & locations, server_context const& s)
{
	std::string word = getWord(curr_pos, end, '{');
	if (!validDirective(word) || getWord(curr_pos, end, '{') != "{")
		throw "Config file error!";

	location_context l;
	initLocation(l, s);
	l.location_name = word;
	while ((word = getWord(curr_pos, end, '}')) != "}" && curr_pos != end)
	{
		if (word == "root")
			setPath(curr_pos, end, l.root);
		else if (word == "index")
			setArray(curr_pos, end, l.index, validDirective);
		else if (word == "client_max_body_size")
			setMaxBodySize(curr_pos, end, l.client_max_body_size);
		else if (word == "http_method")
			setArray(curr_pos, end, l.http_method, validMethod);
		else if (word == "autoindex")
			setEnable(curr_pos, end, l.autoindex);
		else if (word == "default_file_path")
			setPath(curr_pos, end, l.default_file_path);
		else if (word == "cgi_extension")
			setCgiExtension(curr_pos, end, l.cgi_extension);
		else if (word == "cgi_path")
			setPath(curr_pos, end, l.cgi_path);
		else if (word == "uploading_enable")
			setEnable(curr_pos, end, l.uploading_enable);
		else if (word == "uploading_path")
			setPath(curr_pos, end, l.uploading_path);
		else if (word == "return")
			setRedirect(curr_pos, end, l.redirect);
		else
			throw "Config file error!";
	}
	if (word != "}")
		throw "Config file error!";

	locations_map::iterator it = locations.find(l.location_name);
	if (it == locations.end())
		locations.insert( std::make_pair(l.location_name, l) );
	else
		throw "Config file error!";
}

void Config::initServer(server_context & s)
{
	s.host = "*";
	s.port = 80;
	s.root = "html";
	s.index.push_back("index.html");
	s.autoindex = false;
	s.client_max_body_size = toBytes("1m");
	s.server_name.push_back("");
}

void Config::saveAndPassLocation(str_it & curr_pos, str_it const& end, locations_save_list & lsl)
{
	lsl.push_back(curr_pos);
	while (curr_pos != end && *curr_pos != '}')
		curr_pos++;
	if (curr_pos != end)
		curr_pos++;
}

void Config::setServer(str_it & curr_pos, str_it const& end, servers_map & servers)
{
	std::string word = "";
	server_context s;
	initServer(s);
	locations_save_list lsl;

	if (getWord(curr_pos, end) != "{")
		throw "Config file error!";
	while ((word = getWord(curr_pos, end, '}')) != "}" && curr_pos != end)
	{
		if (word == "listen")
			setHostPort(curr_pos, end, s);
		else if (word == "root")
			setPath(curr_pos, end, s.root);
		else if (word == "index")
			setArray(curr_pos, end, s.index, validDirective);
		else if (word == "client_max_body_size")
			setMaxBodySize(curr_pos, end, s.client_max_body_size);
		else if (word == "autoindex")
			setEnable(curr_pos, end, s.autoindex);
		else if (word == "server_name")
			setArray(curr_pos, end, s.server_name, validDirective);
		else if (word == "error_page")
			setErrorPage(curr_pos, end, s.error_page);
		else if (word == "location")
			saveAndPassLocation(curr_pos, end, lsl);
		else if (word == "return")
			setRedirect(curr_pos, end, s.redirect);
		else
			throw "Config file error!";
	}
	if (word != "}")
		throw "Config file error!";
	for (locations_save_list::iterator curr = lsl.begin(); curr != lsl.end(); ++curr)
		setLocation(*curr, end, s.location, s);
	servers[s.host + ":" + all_toa(s.port)].push_back(s);
}

std::string Config::getFileContent(char const* config_path)
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

std::string Config::getWord(str_it & curr_pos, str_it const& end, char delimiter)
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
