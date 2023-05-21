#include "globals.hpp"

namespace glo {

bool            is_cmd_all          = false;
bool            is_dry_run_mode     = false;
bool            is_quiet_mode       = false;
bool            is_recursive_mode   = true;

std::string     base_path           = ".";
std::string     command;
std::string     remote_host;
std::string     remote_type			= "ssh";
std::string     remote_port			= "22";
std::string     remote_user			= "root";
std::string     suffix_str          = ".ope.json";
std::string     working_dir         = ".";

const std::string SSH_OPTS 			= "-o \"StrictHostKeyChecking no\" -o \"UserKnownHostsFile /dev/null\" -o \"GlobalKnownHostsFile  /dev/null\"";
const std::string field_file 		= "file";
const std::string field_local 		= "local";
const std::string field_remote 		= "remote";
const std::string field_bash 		= "bash";

vector_string_t arg_list;
vector_string_t cmd_path;
vector_string_t json_list;
vector_string_t tag_exclude;

}; // glo