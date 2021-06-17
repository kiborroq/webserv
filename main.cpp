#include "Logger.hpp"
#include "Config.hpp"
#include "WebServer.hpp"

int main(int argc, char**argv, char**envp)
{
	(void)argc;
	(void)argv;
	(void)envp;

	try
	{
		Config config("/home/kiborrok/Desktop/git/webserv/test.conf");
		config.parseConfig();

		WebServer ws(config.getServersMap());
		ws.mainLoop();
	}
	catch (ProgramException const& pe)
	{
		Logger(pe.what(), KO);
	}
	return 0;
}
