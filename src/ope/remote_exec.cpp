#include "remote_exec.hpp"
#include "../globals.hpp"
#include "../tools.hpp"

#include <sysexits.h>
#include <unistd.h>

namespace ope {

// --------------------------------------------------------------------

int remote_exec (Json::Value & ope)
{
	if (glo::remote_host.empty())
	{
		return EX_NOHOST;
	}

	if (ope.isMember(glo::field_remote) && ope[glo::field_remote].isString() && !ope[glo::field_remote].asString().empty())
	{
		// Remote execution of a remote script
		const std::string cmd_remote = ope[glo::field_remote].asString();
		const std::string cmd_args = get_args(ope, ope);
		const std::string cmd = std::string("") + "ssh -p " + glo::remote_port + " -T " + glo::SSH_OPTS + " " + glo::remote_user + "@" + glo::remote_host + " " + cmd_remote + " " + cmd_args;
		return log_exec(cmd);
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
			std::string cmd = std::string("") + "ssh -p " + glo::remote_port + " -T " + glo::SSH_OPTS + " " + glo::remote_user + "@" + glo::remote_host + " '" + glo::field_bash + " --login' < " + temp;
			result = log_exec(cmd);
			unlink(temp);
			return result;
		}
		else
		{
			return EX_TEMPFAIL; // 75
		}
	}
	else if (ope.isMember(glo::field_local) && ope[glo::field_local].isString() && !ope[glo::field_local].asString().empty())
	{
		// Remote execution of a local script
		const std::string cmd_local = ope[glo::field_local].asString();
		const std::string cmd_path = glo::working_dir + "/" + cmd_local;
		const std::string cmd_args = get_args(ope, ope);
		if (file_exists(cmd_path))
		{
			const std::string cmd = std::string("") + "ssh -p " + glo::remote_port + " -T " + glo::SSH_OPTS + " " + glo::remote_user + "@" + glo::remote_host + " '" + glo::field_bash + " --login' < " + cmd_path;
			return log_exec(cmd);
		}
		return EXIT_FAILURE;
	}
	else
	{
		return EX_DATAERR;
	}

}

// --------------------------------------------------------------------

} // namespace ope
