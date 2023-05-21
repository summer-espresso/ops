#include "rsync_to.hpp"
#include "../globals.hpp"
#include "../tools.hpp"

#include <sysexits.h>
#include <iostream>

namespace ope {

// --------------------------------------------------------------------

int rsync_to_local (Json::Value & ope)
{
	if (glo::remote_host.empty())
	{
		return EX_NOHOST;
	}

	const std::string cmd_local = ope[glo::field_local].asString();
	const std::string cmd_remote = ope[glo::field_remote].asString();
	const std::string cmd_args = ope["args"].asString();

	if (cmd_local.empty() || cmd_remote.empty())
	{
		return EX_DATAERR;
	}

	const std::string cmd = "rsync " + cmd_args + " -e 'ssh -p " + glo::remote_port + " " + glo::SSH_OPTS + "' \"" + glo::remote_user + "@" + glo::remote_host + ":" + cmd_remote + "\" '" + cmd_local + "'";

	std::cout << cmd << std::endl;

	return log_exec(cmd);
}

// --------------------------------------------------------------------

int rsync_to_remote (Json::Value & ope)
{
	if (glo::remote_host.empty())
	{
		return EX_NOHOST;
	}

	const std::string cmd_local = ope[glo::field_local].asString();
	const std::string cmd_remote = ope[glo::field_remote].asString();
	const std::string cmd_args = ope["args"].asString();

	if (cmd_local.empty() || cmd_remote.empty())
	{
		return EX_DATAERR;
	}

	const std::string cmd = "rsync " + cmd_args + " -e 'ssh -p " + glo::remote_port + " " + glo::SSH_OPTS + "' '" + cmd_local + "' \"" + glo::remote_user + "@" + glo::remote_host + ":" + cmd_remote + "\"";

	std::cout << cmd << std::endl;

	return log_exec(cmd);
}

// --------------------------------------------------------------------

} // namespace ope
