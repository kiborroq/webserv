#include "Config.hpp"

Config::Config(char const* config_path)
	: config_path(config_path), servers()
{ }

Config::~Config(void)
{ }

Config::servers_map const& Config::getServersMap(void)
{ return servers; }

void Config::parseConfig(void)
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

void Config::setHostPort(str_it & curr_pos, str_it const& end, server_context & s)
{
	std::string host_port = getWord(curr_pos, end, ';');
	if (host_port == ";" || getWord(curr_pos, end, ';') != ";")
		throw "Config 2";

	str_it curr_hp = host_port.begin();
	str_it end_hp = host_port.end();

	if (iscontained(host_port, ':'))
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
		if (std::find(l.begin(), l.end(), elem) != l.begin())
			l.push_back(elem);
		elem = getWord(curr_pos, end, ';');
	}
	if (curr_pos == end)
		throw "Config file error!";
}

void Config::setMaxBodySize(str_it & curr_pos, str_it const& end, size_t & client_max_body_size)
{
	std::string size = getWord(curr_pos, end, ';');
	if (!validMaxBodySize(size) || getWord(curr_pos, end, ';') != ";")
		throw "Config file error!";

	if (isalpha(*(size.end() - 1)))
		client_max_body_size = tobytes(size);
	else
		client_max_body_size = ato_all<size_t>(size);
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
	location_counter counter;
	memset(&counter, 0, sizeof(counter));

	while ((word = getWord(curr_pos, end, '}')) != "}" && curr_pos != end)
	{
		if (word == "root" && counter.root++ < 1)
			setPath(curr_pos, end, l.root);
		else if (word == "index")
			setArray(curr_pos, end, l.index, validDirective);
		else if (word == "client_max_body_size" && counter.client_max_body_size++ < 1)
			setMaxBodySize(curr_pos, end, l.client_max_body_size);
		else if (word == "http_method")
			setArray(curr_pos, end, l.http_method, validMethod);
		else if (word == "autoindex" && counter.autoindex++ < 1)
			setEnable(curr_pos, end, l.autoindex);
		else if (word == "default_file_path" && counter.default_file_path++ < 1)
			setPath(curr_pos, end, l.default_file_path);
		else if (word == "cgi_extension" && counter.cgi_extension++ < 1)
			setCgiExtension(curr_pos, end, l.cgi_extension);
		else if (word == "cgi_path" && counter.cgi_path++ < 1)
			setPath(curr_pos, end, l.cgi_path);
		else if (word == "uploading_enable" && counter.uploading_enable++ < 1)
			setEnable(curr_pos, end, l.uploading_enable);
		else if (word == "uploading_path" && counter.uploading_path++ < 1)
			setPath(curr_pos, end, l.uploading_path);
		else if (word == "return" && counter.redirect++ < 1)
			setRedirect(curr_pos, end, l.redirect);
		else
			throw "Config file error!";
	}
	if (word != "}")
		throw "Config file error!";

	if (locations.find(l.location_name) == locations.end())
		locations[l.location_name] = l;
	else
		throw "Config file error!";
}

void Config::initServer(server_context & s)
{
	s.host = "localhost";
	s.port = 80;
	s.root = "html";
	s.index.push_back("index.html");
	s.autoindex = false;
	s.client_max_body_size = tobytes("1m");
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
	server_counter counter;
	memset(&counter, 0, sizeof(counter));

	if (getWord(curr_pos, end) != "{")
		throw "Config file error!";
	while ((word = getWord(curr_pos, end, '}')) != "}" && curr_pos != end)
	{
		if (word == "listen" && counter.listen++ < 1)
			setHostPort(curr_pos, end, s);
		else if (word == "root" && counter.root++ < 1)
			setPath(curr_pos, end, s.root);
		else if (word == "index")
			setArray(curr_pos, end, s.index, validDirective);
		else if (word == "client_max_body_size" && counter.client_max_body_size++ < 1)
			setMaxBodySize(curr_pos, end, s.client_max_body_size);
		else if (word == "autoindex" && counter.autoindex++ < 1)
			setEnable(curr_pos, end, s.autoindex);
		else if (word == "server_name")
			setArray(curr_pos, end, s.server_name, validDirective);
		else if (word == "error_page")
			setErrorPage(curr_pos, end, s.error_page);
		else if (word == "location" && ++counter.location)
			saveAndPassLocation(curr_pos, end, lsl);
		else if (word == "return" && counter.redirect++ < 1)
			setRedirect(curr_pos, end, s.redirect);
		else
			throw "Config file error!";
	}
	if (word != "}" || (counter.location == 0 && counter.redirect == 0))
		throw "Config file error!";

	std::string server_id = s.host + ":" + all_toa(s.port);
	if (servers.find(server_id) == servers.end()) {
		for (locations_save_list::iterator curr = lsl.begin(); curr != lsl.end(); ++curr)
			setLocation(*curr, end, s.location, s);
		servers[server_id] = s;
	}
}
