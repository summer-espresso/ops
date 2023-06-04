#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <string>
#include <vector>

using vector_string_t = std::vector<std::string>;

#define MIN_ARG_LIST 2

namespace glo {

extern bool is_cmd_all;
extern bool is_dry_run_mode;
extern bool is_quiet_mode;
extern bool is_recursive_mode;
extern bool is_reverse_mode;

extern std::string base_path;
extern std::string command;
extern std::string remote_host;
extern std::string remote_port;
extern std::string remote_type;
extern std::string remote_user;
extern std::string suffix_str;
extern std::string working_dir;

extern const std::string SSH_OPTS;
extern const std::string field_file;
extern const std::string field_local;
extern const std::string field_remote;
extern const std::string field_bash;

extern vector_string_t arg_list;
extern vector_string_t cmd_path;
extern vector_string_t json_list;
extern vector_string_t tag_exclude;

}; // glo


#endif // GLOBALS_HPP