#include "Logger.hpp"

Logger::Logger(std::string const& message, std::string const& status)
{
	std::cout 	<< status
				<< "[ " <<  getTime() << " ]: "
				<< message << std::endl;
}

std::string Logger::getTime(void)
{
	time_t rawtime;
	struct tm *timeinfo;

	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);

	return 	all_toa(timeinfo->tm_mday) + "." +
			all_toa(timeinfo->tm_mon + 1) + "." +
			all_toa(1900 + timeinfo->tm_year) + " " +
			all_toa(timeinfo->tm_hour) + ":" +
			all_toa(timeinfo->tm_min);
}
