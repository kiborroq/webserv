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
# include "parsing_utils.hpp"

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

typedef struct						location_counter
{
	size_t							root = 0;
	size_t							index = 0;
	size_t							client_max_body_size = 0;
	size_t							http_method = 0;
	size_t							autoindex = 0;
	size_t							default_file_path = 0;
	size_t							cgi_extension = 0;
	size_t							cgi_path = 0;
	size_t							uploading_enable = 0;
	size_t							uploading_path = 0;
	size_t							redirect = 0;
}									location_counter;

typedef struct						server_counter
{
	size_t							listen = 0;
	size_t							root = 0;
	size_t							index = 0;
	size_t							autoindex = 0;
	size_t							client_max_body_size = 0;
	size_t							server_name = 0;
	size_t							error_page = 0;
	size_t							redirect = 0;
}									server_counter;

class Config
{
	private:
		typedef std::map< std::string, std::list<server_context> > servers_map;
		typedef std::map<std::string, location_context> locations_map;
		typedef std::map<int, std::string> errorpages_map;
		typedef std::list<str_it> locations_save_list;
	
	public:
		servers_map servers;
	
	public:
		Config(char const* config_path);
		~Config(void);
	
	private:
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
	
		void initServer(server_context & s);
		void initLocation(location_context & l, server_context const& src);
		void saveAndPassLocation(str_it & curr_pos, str_it const& end, locations_save_list & lsl);
};

#endif // CONFIGFILE_HPP
