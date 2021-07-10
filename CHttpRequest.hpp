#ifndef FSM_CHTTPREQUEST_HPP
#define FSM_CHTTPREQUEST_HPP
#include <string>
#include <map>

#define FSM_OK 0
#define FSM_AGAIN 1


class CHttpRequest
{
private:
    std::string _message;
    size_t _pos;
    size_t _old_pos;
	typedef enum e_fsm_state
	{
		fsm_ok = 0,
		fsm_again,
		fsm_start = 100,
		fsm_read_request_line,
		fsm_read_header,
		fsm_read_data,
		fsm_prepare_answer,
		fsm_set_redirection,
		fsm_send_redirection,
		fsm_invalid_method,
		fsm_invalid_request,
		fsm_invalid_header,
		fsm_http_09
	} t_fsm_state;
	t_fsm_state _fsm_state;
    enum
    {
        sw_start = 0,
        sw_method,
        sw_spaces_before_uri,
        sw_after_slash_in_uri,
        sw_schema,
        sw_schema_slash,
        sw_schema_slash_slash,
        sw_host,
        sw_port,
        sw_path,
        sw_query,
        sw_fragment,
        sw_after_uri,
        sw_after_cr,
        sw_name,
        sw_spaces_after_colon,
        sw_header_end_cr,
        sw_header_done,
        sw_field_cr,
        sw_value,
        sw_data_cr
    } _state;
	enum
	{
		HTTP_GET = 0,
		HTTP_POST,
		HTTP_DELETE
	} _method;
	enum {
		HTTP_SCHEMA = 0,
		HTTPS_SCHEMA
	} _schema;
    std::string _host;
    int _port;
    std::string _path;
    std::string _query;
    std::string _fragment;
    std::string _http_ver;
    std::string _data;
    std::map <std::string, std::string> _header;
	t_fsm_state parse_request_line();
	t_fsm_state parse_header_line();
	t_fsm_state read_data(size_t size);
	bool isTchar(char ch);

public:
	CHttpRequest();
	~CHttpRequest();
	int handleHttpRequest(const std::string &message);
	void printRequest();
};


#endif //FSM_CHTTPREQUEST_HPP
