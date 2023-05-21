#include "local_exec.hpp"
#include "../globals.hpp"
#include "../tools.hpp"

#include <sysexits.h>
#include <unistd.h>

namespace ope {

// --------------------------------------------------------------------

int local_exec (Json::Value & ope)
{
	if (ope.isMember(glo::field_local) && ope[glo::field_local].isString() && !ope[glo::field_local].asString().empty())
	{
		const std::string cmd_path = glo::working_dir + "/" + ope[glo::field_local].asString();
		const std::string cmd_args = get_args(ope, ope);
		return log_exec(cmd_path + " " + cmd_args);
	}
	else if (ope.isMember(glo::field_bash) && ope[glo::field_bash].isString() && !ope[glo::field_bash].asString().empty())
	{
		int result = EXIT_FAILURE;
		char temp[] = "/tmp/exec-XXXXXX";
		int fd = mkstemp(temp);

		if (fd > -1)
		{
			const std::string shell = ope[glo::field_bash].asString();
			write(fd, shell.c_str(), shell.length());
			close(fd);
			std::string cmd = glo::field_bash + " " + temp;
			result = log_exec(cmd);
			unlink(temp);
			return result;
		}
		else
		{
			return EX_TEMPFAIL; // 75
		}
	}

	return EXIT_FAILURE;
}

// --------------------------------------------------------------------

} // namespace ope
