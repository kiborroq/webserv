/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiborroq <kiborroq@kiborroq.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/03 09:34:55 by kiborroq          #+#    #+#             */
/*   Updated: 2021/06/06 22:13:07 by kiborroq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <map>
# include <list>
# include <string>
# include <vector>
# include <iostream>
# include <fstream>
# include <sstream>
# include <limits>
# include "utils.hpp"

typedef struct						location_context
{
    std::string						location_name;
    std::string						root;
    std::list<std::string>			index;
    size_t							client_max_body_size;
    std::list<std::string>			http_method;
    bool							autoindex;
    std::string						default_file_path;
    std::string						cgi_extension;
    std::string						cgi_path;
    bool							uploading_enable;
    std::string						uploading_path;
	std::pair<int, std::string>		redirect;
}									location_context;

typedef struct						server_context
{
    std::string						host;
    size_t							port;
    std::string						root;
    std::list<std::string>			index;
    bool							autoindex;
    size_t							client_max_body_size;
    std::list<std::string>			server_name;
    std::map<int, std::string>		error_page;
    std::map<std::string, location_context>	location;
    std::pair<int, std::string>		redirect;
} 									server_context;

class Config
{
private:
    typedef std::map< std::string, std::list<server_context> > servers_map;
    typedef std::map<std::string, location_context> locations_map;
    typedef std::map<int, std::string> errorpages_map;
    typedef std::string::iterator str_it;
    typedef std::string::const_iterator str_const_it;
    typedef std::list<str_it> locations_save_list;

public:
    size_t const UINT64_MAX;
    size_t const UINT16_MAX;
    servers_map servers;

public:
    Config(char const* config_path);
    ~Config(void);

private:
    std::string getFileContent(char const* config_path);
    std::string getWord(str_it & curr_pos, str_it const& end, char delimiter = ' ');
    void setServer(str_it & curr_pos, str_it const& end, servers_map & servers);
    void setHostPort(str_it & curr_pos, str_it const& end, server_context & s);
    void setPath(str_it & curr_pos, str_it const& end, std::string & path);
    void setMaxBodySize(str_it & curr_pos, str_it const& end, size_t & client_max_body_size);
    void setErrorPage(str_it & curr_pos, str_it const& end, errorpages_map & error_pages);
    void setArray(str_it & curr_pos, str_it const& end, std::list<std::string> & l, bool (*valid)(std::string const& s));
    void setEnable(str_it & curr_pos, str_it const& end, bool & enable);
    void setCgiExtension(str_it & curr_pos, str_it const& end, std::string & extension);
    void setLocation(str_it & curr_pos, str_it const& end, locations_map & locations, server_context const& s);
	void setRedirect(str_it & curr_pos, str_it const& end, std::pair<int, std::string> & redirect);
	bool validHost(std::string & host);
    bool validMaxBodySize(std::string const& size);
	bool validCgiExtension(std::string const& extension);
    void initServer(server_context & s);
    void initLocation(location_context & l, server_context const& src);
    void saveAndPassLocation(str_it & curr_pos, str_it const& end, locations_save_list & lsl);
    // bool validDirective(std::string const& value);
    bool validHttpCode(std::string const& code, int lower_bound = 0, int upper_bound = 999);
    bool validPort(std::string const& port);

    size_t toBytes(std::string const& size);
    bool isonlydigits(std::string const& s, size_t count);
    // bool does_contain(std::string const& s, char c);
    friend std::ostream & operator<<(std::ostream & out, Config const & cf);
};

#endif // CONFIGFILE_HPP
