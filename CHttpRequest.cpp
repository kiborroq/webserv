#include "CHttpRequest.hpp"
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>

int CHttpRequest::handleHttpRequest(const std::string &message)
{
	t_fsm_state result;
	size_t size;

	_message += message;
	while(true)
	{
		switch (_fsm_state)
		{
			case fsm_start:
			case fsm_read_request_line:
				result = parse_request_line();
				switch (result)
				{
					case fsm_ok:
						_state = sw_start;
						_fsm_state = fsm_read_header;
						break;
					case fsm_again:
						return FSM_AGAIN;
					default:
						_fsm_state = result;
				}
				break;
			case fsm_read_header:
				result = parse_header_line();
				switch (result)
				{
					case fsm_ok:
						if (_method == HTTP_GET && _header["content-length"].length() != 0)
							_fsm_state = fsm_read_data;
						else
							_fsm_state = fsm_prepare_answer;
						break;
					case fsm_again:
						return FSM_AGAIN;
					default:
						_fsm_state = result;
						break;
				}
				break;

			case fsm_read_data:
				size = std::atoi(_header["content-length"].c_str());
				result = read_data(size);
				if (result == fsm_ok)
				{
					_fsm_state = fsm_prepare_answer;
					break;
				}
				if (result == fsm_again)
					return FSM_AGAIN;
				break;

			case fsm_prepare_answer:
				return FSM_OK;
		}
	}
}

CHttpRequest::t_fsm_state CHttpRequest::parse_request_line()
{
	const std::string &m = _message;
	for (;_pos < _message.length(); _pos++)
	{
		switch (_state)
		{
			case sw_start:
				if (m[_pos] == '\r' || m[_pos] == '\n')
					break;
				if (!std::isalpha(m[_pos]))
					return fsm_invalid_method;
				_old_pos = _pos;
				_state = sw_method;
				break;

			case sw_method:
				if (m[_pos] == ' ')
				{
					if (!m.compare(_old_pos, 4, "GET "))
						_method = HTTP_GET;
					else if (!m.compare(_old_pos, 5, "POST "))
						_method = HTTP_POST;
					else if (!m.compare(_old_pos, 7, "DELETE "))
						_method = HTTP_DELETE;
					else
						return fsm_invalid_method;
					_state = sw_spaces_before_uri;
				}
				break;

			case sw_spaces_before_uri:
				if (m[_pos] == '/')
				{
					_state = sw_path;
					_old_pos = _pos + 1;
					break;
				} else if (std::isalpha(m[_pos]))
				{
					_state = sw_schema;
					_old_pos = _pos;
					break;
				} else if (m[_pos] == ' ')
					break;
				else
					return fsm_invalid_request;

			case sw_after_slash_in_uri: // TODO:


			case sw_schema:
				if (isalpha(m[_pos]))
					break;
				else if (m[_pos] == ':')
				{
					if (!_message.compare(_old_pos, 5, "http:"))
						_schema = HTTP_SCHEMA;
					else if (!_message.compare(_old_pos, 6, "https:"))
						_schema = HTTPS_SCHEMA;
					else
						return fsm_invalid_request;
					_state = sw_schema_slash;
					break;
				} else
					return fsm_invalid_request;

			case sw_schema_slash:
				if (m[_pos] == '/')
				{
					_state = sw_schema_slash_slash;
					break;
				} else
					return fsm_invalid_request;

			case sw_schema_slash_slash:
				if (m[_pos] == '/')
				{
					_state = sw_host;
					_old_pos = _pos + 1;
					break;
				} else
					return fsm_invalid_request;

			case sw_host:
				if (std::isalnum(m[_pos]) || m[_pos] == '.' || m[_pos] == '-')
					break;
				if (_pos != _old_pos)
					_host = m.substr(_old_pos, _pos - _old_pos);
				switch (m[_pos])
				{
					case ' ':
						_old_pos = _pos + 1;
						_state = sw_after_uri;
						break;
					case ':':
						_state = sw_port;
						_old_pos = _pos + 1;
						break;
					case '/':
						_state = sw_path;
						_old_pos = _pos + 1;
						break;
					default:
						return fsm_invalid_request;
				}
				break;
			case sw_port:
				if (std::isdigit(m[_pos]))
					break;
				if (_pos != _old_pos)
					_port = std::atoi(m.substr(_old_pos, _pos - _old_pos).c_str());
				switch (m[_pos])
				{
					case ' ':
						_old_pos = _pos + 1;
						_state = sw_after_uri;
						break;
					case '/':
						_old_pos = _pos + 1;
						_state = sw_path;
						break;
					default:
						return fsm_invalid_request;
				}
				break;
			case sw_path:
				if (std::isprint(m[_pos]) && m[_pos] != '?' && m[_pos] != '#' &&
					m[_pos] != ' ')
					break;
				if (_pos != _old_pos)
					_path = m.substr(_old_pos, _pos - _old_pos);
				switch (m[_pos])
				{
					case ' ':
						_old_pos = _pos + 1;
						_state = sw_after_uri;
						break;
					case '\r':
					case '\n':
						return fsm_http_09;
					case '?':
						_old_pos = _pos + 1;
						_state = sw_query;
						break;
					case '#':
						_old_pos = _pos + 1;
						_state = sw_fragment;
						break;
					default:
						return fsm_invalid_request;
				}
				break;
			case sw_query:
				if (std::isprint(m[_pos]) && m[_pos] != '#' && m[_pos] != ' ')
					break;
				if (_pos != _old_pos)
					_query = m.substr(_old_pos, _pos - _old_pos);
				switch (m[_pos])
				{
					case ' ':
						_old_pos = _pos + 1;
						_state = sw_after_uri;
					case '\r':
					case '\n':
						return fsm_http_09;
					case '#':
						_old_pos = _pos + 1;
						_state = sw_fragment;
						break;
					default:
						return fsm_invalid_request;
				}
				break;
			case sw_fragment:
				if (std::isprint(m[_pos]) && m[_pos] != '#' && m[_pos] != ' ')
					break;
				if (_pos != _old_pos)
					_fragment = m.substr(_old_pos, _pos - _old_pos);
				switch (m[_pos])
				{
					case ' ':
						_old_pos = _pos + 1;
						_state = sw_after_uri;
						break;
					case '\r':
					case '\n':
						return fsm_http_09;
					default:
						return fsm_invalid_request;
				}
				break;
			case sw_after_uri:
				if (std::isprint(m[_pos]) && m[_pos] != ' ')
					break;
				switch (m[_pos])
				{
					case ' ':
						_old_pos = _pos + 1;
						break;
					case '\r':
						if (_pos != _old_pos)
							_http_ver = m.substr(_old_pos, _pos - _old_pos);
						_state = sw_after_cr;
						break;
					default:
						return fsm_invalid_request;
				}
				break;
			case sw_after_cr:
				switch (m[_pos])
				{
					case '\n':
						if (_http_ver == "HTTP/1.1" && m[_pos] == '\n')
						{
							_pos++;
							return fsm_ok;
						}
					default:
						return fsm_invalid_request;
				}
		}
	}
	return fsm_again;
}

CHttpRequest::CHttpRequest():_pos(0), _old_pos(0), _fsm_state(fsm_start), _state(sw_start), _method(HTTP_GET), _schema(HTTP_SCHEMA), _port(0)
{

}

CHttpRequest::~CHttpRequest()
{

}

CHttpRequest::t_fsm_state CHttpRequest::parse_header_line()
{
	static std::string name;
	static std::string value;
	const std::string &m = _message;
	for (;_pos < _message.length(); _pos++)
		switch (_state)
		{
			case sw_start:
				name.clear();
				value.clear();
				if (m[_pos] == '\r')
				{
					_state = sw_header_end_cr;
					break;
				}
				_old_pos = _pos;
				_state = sw_name;

			case sw_name:
				if (isTchar(m[_pos]))
					break;
				if (_pos != _old_pos)
				{
					name = m.substr(_old_pos, _pos - _old_pos);
					std::transform(name.begin(), name.end(), name.begin(), ::tolower);
				}
				switch (m[_pos])
				{
					case ':':
						_state = sw_spaces_after_colon;
						break;
					case '\r':
						_state = sw_field_cr;
					default:
						return fsm_invalid_header;
				}
				break;

			case sw_header_end_cr:
				if (m[_pos] == '\n')
				{
					_pos++;
					return fsm_ok;
				}
				else return fsm_invalid_header;

			case sw_field_cr:
				if (m[_pos] == '\n')
				{
					_state = sw_start;
					_header[name] += value;
					break;
				}
				else return fsm_invalid_header;

			case sw_spaces_after_colon:
				switch (m[_pos])
				{
					case ' ':
						break;
					case '\r':
						_state = sw_field_cr;
						break;
					default:
						_old_pos = _pos;
						_state = sw_value;
						break;
				}
				break;

			case sw_value:
				if (std::isprint(m[_pos]) || m[_pos] < 0 || m[_pos] == ' ' || m[_pos] == '\t')
					break;
				switch (m[_pos])
				{
					case '\r':
						if (_pos != _old_pos)
							value = m.substr(_old_pos, _pos - _old_pos);
					_state = sw_field_cr;
				}
				break;
		}
	return fsm_again;
}

bool CHttpRequest::isTchar(char ch)
{
	static u_int32_t t_char[] = {
			 		// URGFESECESNDDDDDSSCFVLTBBAEEESSN
			 		// SSSSSUMATYACCCCLIORFTFASECNOTTOU
			 		//     CB NBNK4321E      B LKQTXXHL
	0x00000000,		// 00000000000000000000000000000000
			 		//                                S
			 		// ?>=<;:9876543210/.-,+*)('&%$#"!P
	0x03ff6cfa,		// 00000011111111110110110011111010
			 		// _^]\[ZYXWVUTSRQPONMLKJIHGFEDCBA@
	0xc7fffffe,		// 11000111111111111111111111111110
			 		// D
			 		// E~}|{zyxwvutsrqponmlkjihgfedcba`
	0x57ffffff,		// 01010111111111111111111111111111
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
	};
	return t_char[ch >> 5] & (1U << (ch & 0x1f));
}

void CHttpRequest::printRequest()
{
	const static char *methods[] = {"GET", "POST", "DELETE"};
	const static char *schema[] = {"http", "https"};
	std::cout << "method: " << methods[_method] << std::endl;
	std::cout << "schema: " << schema[_schema] << std::endl;
	std::cout << "host: " << _host << std::endl;
	std::cout << "port: " << _port << std::endl;
	std::cout << "path: " << _path << std::endl;
	std::cout << "query: " << _query << std::endl;
	std::cout << "fragment: " << _fragment << std::endl;
	std::cout << "http_ver: " << _http_ver << std::endl;
	std::cout << "header:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator iter = _header.begin(); iter != _header.end(); iter++)
		std::cout << "\t" << iter->first << ": " << iter->second << std::endl;
	if (_header["content-length"].length() != 0)
		std::cout << _data << std::endl;
}

CHttpRequest::t_fsm_state CHttpRequest::read_data(size_t size)
{
	const std::string &m = _message;
	for (_old_pos = _pos; _pos < _message.length(); _pos++)
		switch (m[_pos])
		{
			case '\r':
				_state = sw_data_cr;
				break;
			case '\n':
				if (_pos - _old_pos - 1 == size && _state == sw_data_cr)
				{
					_data = m.substr(_old_pos, _pos - _old_pos - 1);
					return fsm_ok;
				}
				else
					return fsm_invalid_request;
			default:
				break;
		}
	return fsm_again;
}


