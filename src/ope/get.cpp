#include "get.hpp"
#include "../globals.hpp"
#include "../tools.hpp"

#include <sysexits.h>

namespace ope {

// --------------------------------------------------------------------

int get (Json::Value & ope)
{
	if (glo::remote_host.empty())
	{
		return EX_NOHOST;
	}

	const std::string cmd_local = ope[glo::field_local].asString();
	const std::string cmd_remote = ope[glo::field_remote].asString();

	if (cmd_local.empty() || cmd_remote.empty())
	{
		return EX_DATAERR;
	}

	const std::string cmd = "scp -T -P " + glo::remote_port + " -p " + glo::SSH_OPTS + " " + glo::remote_user + "@" + glo::remote_host + ":\"'" + cmd_remote + "'\" '" + cmd_local + "'";
	return log_exec(cmd);
}

// --------------------------------------------------------------------

} // namespace ope
