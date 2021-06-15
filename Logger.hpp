#ifndef LOGGER_HPP
#define LOGGER_HPP

# include <string>
# include <ctime>
# include <iostream>
# include "parsing_utils.hpp"

# define OK "[ \033[1;32mOK\033[0m ]"
# define KO "[ \033[1;31mKO\033[0m ]"

class Logger {

	public:
		Logger(std::string const& message, std::string const& status);

	private:
		std::string getTime(void);
};

#endif //LOGGER_HPP
