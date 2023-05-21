#include "process.hpp"
#include "ope/get.hpp"
#include "ope/put.hpp"
#include "ope/local_exec.hpp"
#include "ope/remote_exec.hpp"
#include "ope/rsync_to.hpp"
#include "globals.hpp"
#include "tools.hpp"

#include <algorithm>
#include <iostream>
#include <map>

#include <dirent.h>
#include <sys/stat.h>

#include <json/json.h>

typedef int (*ope_function_t)(Json::Value &);

using map_fn_t = std::map<std::string, ope_function_t>;

map_fn_t map_fn
{
	{ "get", ope::get },
	{ "put", ope::put },
	{ "local_exec", ope::local_exec },
	{ "remote_exec", ope::remote_exec },
	{ "rsync_to_local", ope::rsync_to_local },
	{ "rsync_to_remote", ope::rsync_to_remote },
};

const std::string sep = "\n-----------------------------------------------------------\n";

// --------------------------------------------------------------------

void load_file_list(const std::string search_path)
{
	const size_t suffix_len = glo::suffix_str.length();

	DIR * dir = NULL;
	struct dirent * ent = NULL;
	unsigned char d_type = DT_UNKNOWN;

	if ((dir = opendir(search_path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			d_type = ent->d_type;

			select_type:

			switch (d_type)
			{
				case DT_REG:
				{

					const std::string filename = ent->d_name;
					if ((filename.length() > suffix_len) && (filename.substr(filename.length() - suffix_len, suffix_len) == glo::suffix_str))
					{
						glo::json_list.push_back(search_path + "/" + ent->d_name);
					}
					break;
				}

				case DT_DIR:
				{
					std::string dirname = ent->d_name;
					if ((dirname != ".") && (dirname != ".."))
					{
						load_file_list(search_path + "/" + dirname);
					}
					break;
				}

				case DT_LNK:
				case DT_UNKNOWN:
				{
					if (dir_exists(ent->d_name))
					{
						d_type = DT_DIR;
						goto select_type;
					}
					if (file_exists(ent->d_name))
					{
						d_type = DT_REG;
						goto select_type;
					}
					break;
				}
			}
		}

		closedir(dir);
	}
}

// --------------------------------------------------------------------

void extract_file_list()
{
	vector_string_t new_list (glo::arg_list.begin() + MIN_ARG_LIST, glo::arg_list.end());
	glo::json_list = new_list;
}

// --------------------------------------------------------------------
// Recherche récursive pour trouver la commande finale
// --------------------------------------------------------------------

Json::Value * find_node_command(Json::Value * from)
{
	for (std::string & cmd : glo::cmd_path)
	{
		if (from->isObject() && from->isMember(cmd))
		{
			from = &(*from)[cmd];
		}
		else
		{
			return nullptr;
		}
	}
	return from;
}

// --------------------------------------------------------------------

int operation(Json::Value & ope)
{
	if (!glo::tag_exclude.empty())
	{
		if (ope.isMember("xtag") && ope["xtag"].isString() && !ope["xtag"].asString().empty())
		{
			if (std::find(glo::tag_exclude.begin(), glo::tag_exclude.end(), ope["xtag"].asString()) != glo::tag_exclude.end())
			{
				return EXIT_SUCCESS;
			}
		}
	}

	std::cout << "   : ope : " << (ope.isMember("desc") && ope["desc"].isString() ? ope["desc"].asString() : "") << " : ";
	int exit_code = EXIT_FAILURE;
	if (ope.isMember("type") && ope["type"].isString())
	{
		const std::string ope_type = ope["type"].asString();
		if (!ope_type.empty())
		{
			if (map_fn.find(ope_type) != map_fn.end())
			{
				if (glo::is_dry_run_mode)
				{
					// DRY RUN
					std::cout << "(DRY RUN)";
					exit_code = EXIT_SUCCESS;
				}
				else
				{
					// Execution
					std::cout << sep;
					exit_code = map_fn[ope_type](ope);
					std::cout << sep;
					if (exit_code == 0)
					{
						std::cout << "OK";
					}
					else
					{
						std::cout << "ERROR exit code " << exit_code;
					}
					std::cout << sep;
				}
			}
			else
			{
				std::cout << "(error) unkown type " << ope_type;
			}
		}
		else
		{
			std::cout << "(error) void type";
		}
	}
	else
	{
		std::cout << "(error) type not defined";
	}
	std::cout << "\n";
	return exit_code;
}

// --------------------------------------------------------------------

int exec_one_cmd(Json::Value & cmd)
{
	size_t size = cmd.size();
	for (Json::ArrayIndex idx = 0 ; idx < size ; idx++)
	{
		int exit_code = operation(cmd[idx]);
		if (exit_code != EXIT_SUCCESS)
		{
			std::cout << "Stopped in error (exit code " << exit_code << ")" << std::endl;
			return exit_code;
		}
	}
	return EXIT_SUCCESS;
}

// --------------------------------------------------------------------

void process ()
{
	if (glo::is_recursive_mode)
	{
		load_file_list(glo::base_path);
	}
	else
	{
		extract_file_list();
	}

	std::sort(glo::json_list.begin(), glo::json_list.end());

	for (std::string & json_file : glo::json_list)
	{
		std::cout << " - file : " << json_file;
		Json::Value app = load_json(json_file);
		if (app.isObject() && app.isMember("cmd") && app["cmd"].isObject())
		{
			std::cout << " (loaded)\n";
			Json::Value * cmd = find_node_command(&app["cmd"]) ;
			if (cmd != nullptr)
			{
				if (glo::is_cmd_all)
				{
					if (cmd->isObject())
					{
						for (const std::string & sub : cmd->getMemberNames())
						{
							if ((*cmd)[sub].isArray())
							{
								if (exec_one_cmd((*cmd)[sub]) != EXIT_SUCCESS)
									return;
							}
						}
					}
				}
				else
				{
					if (cmd->isArray())
					{
						if (exec_one_cmd(*cmd) != EXIT_SUCCESS)
							return;
					}
				}
			}
		}
		else
		{
			// IGNORED
		}
	}
}

// --------------------------------------------------------------------
