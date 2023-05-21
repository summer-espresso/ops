#include "globals.hpp"
#include "process.hpp"
#include "tools.hpp"

#include <iostream>
#include <sysexits.h>
#include <unistd.h>

void version ()
{
	std::cout << "0.1.0" << std::endl;
}

void use ()
{
	std::cerr << ""
		"Operational Personal System\n"
		"\n"
		"Use:\n"
		"  ops [options] cmd [command] [json_files]\n"
		"\n"
		"options:\n"
		"  -a Address or hostname of the remote host. Must be provided with the ssh connection (by default).\n"
		"  -b Base path for .ops.json files. Default is the current directory.\n"
		"  -c Remote connection type. Default is 'ssh'. Legal values are 'ssh', 'chroot', 'local'\n"
		"  -d Dry run. Do nothing.\n"
		"  -h Display this help.\n"
		"  -p Port of the remote connection.\n"
		"  -q Quiet mode.\n"
		"  -s Suffix of operations files. Default is '.ope.json'\n"
		"  -u Remote user for the connection. Default is 'root'"
		"  -v Display version.\n"
		"  -w Working directory. Default is the current directory.\n"
		"  -x [tags] Exclude tags. Do not execute operation containing this tags.\n"
		"\n"
		"command:\n"
		"  Child node under the cmd node of the json file.\n"
		"  - The slash separator '/' designates a hierarchy of childs.\n"
		"  - The suffix ':all' designates all child of a hierarchy of childs.\n"
		"\n"
		"    Example : 'install' command designates the child node 'install' under the node 'cmd'\n"
		"    {\n"
		"      \"cmd\":\n"
		"      {\n"
		"        \"install\": {}\n"
		"      }\n"
		"    }\n"
		"\n"
		"    Example : 'backup/data'\n"
		"    {\n"
		"      \"cmd\":\n"
		"      {\n"
		"        \"backup\":\n"
		"        {\n"
		"          \"data\": {}\n"
		"        }\n"
		"      }\n"
		"    }\n"
		"\n"
		"    Example : 'backup:all' designates all the children under the backup node\n"
		"\n"
		"\n"
		<< std::flush
		;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		use();
		return EX_USAGE; // 64
	}

	char list_opt[] = "a:b:c:dhp:qs:u:vw:x:";
	int opt = 0;
	std::string log_level;

	while ((opt = getopt(argc, argv, list_opt)) != -1)
	{
		switch (opt)
		{
		case 'a':
			glo::remote_host = optarg;
			break;
		case 'b':
			glo::base_path = optarg;
			break;
		case 'c':
			glo::remote_type = optarg;
			break;
		case 'd':
			glo::is_dry_run_mode = true;
			break;
		case 'h':
			use();
			return 0;
			break;
		case 'p':
			glo::remote_port = optarg;
			break;
		case 'q':
			glo::is_quiet_mode = true;
			break;
		case 's':
			glo::suffix_str = optarg;
			break;
		case 'u':
			glo::remote_user = optarg;
			break;
		case 'v':
			version();
			break;
		case 'w':
			glo::working_dir = optarg;
			break;
		case 'x':
			glo::tag_exclude = tokenize_string(std::string(optarg), ' ');
			break;
		}
	}

	if (optind < argc)
	{
		while (optind < argc)
		{
			glo::arg_list.push_back(argv[optind++]);
		}
	}
	else
	{
		std::cerr << "Exit in error. You must provide a command" << std::endl;
		return EX_USAGE; // 64
	}

	if (!glo::tag_exclude.empty())
	{
		std::cout << "tag filter : " << glo::tag_exclude.size() << "\n";
	}

	if (glo::arg_list.size() < 2)
	{
		std::cerr << "Not enough parameters provided" << std::endl;
		use();
		return EX_USAGE; // 64
	}

	if (glo::remote_type != "ssh" && glo::remote_type != "chroot" )
	{
		std::cerr << "Remote connection type value not valid" << std::endl;
		return EX_USAGE; // 64
	}

	if (glo::remote_type == "ssh" && glo::remote_host.empty() )
	{
		std::cerr << "You must provide the remote host name or address (-a) with the ssh remote connection type" << std::endl;
		return EX_USAGE; // 64
	}

	glo::command = glo::arg_list[1];
	if ((glo::command.size() > 4) && (glo::command.substr(glo::command.size() - 4, 4) == ":all"))
	{
		glo::command = glo::command.substr(0, glo::command.size() - 4);
		glo::is_cmd_all = true;
	}
	glo::cmd_path = tokenize_string(glo::command, '/');

	std::cout << "command    : " << glo::command << "\n";
	std::cout << "suffix     : " << glo::suffix_str << "\n";

	glo::is_recursive_mode = glo::arg_list.size() <= MIN_ARG_LIST;

	std::cout << "recursive  : " << (glo::is_recursive_mode ? "yes" : "no") << "\n";

	process();

    return 0;
}
