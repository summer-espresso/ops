#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <vector>

#include <json/json.h>

using vector_string_t = std::vector<std::string>;

namespace glo
{
}; // namespace glo

enum class file_type_t
 {
    FOLDER,
    TASK,
    ERROR,
};

Json::Value     decode_json         (const std::string value);
Json::Value     load_json           (const std::string filename);
void            save_json           (const std::string filename, Json::Value & node);
std::string     load_content        (const std::string filename);
bool            dir_exists          (const std::string folder);
bool            file_exists         (const std::string filename);
vector_string_t tokenize_string		(const std::string & source_string, const char separator);
std::string     get_args            (Json::Value & param, Json::Value & ope);
int 			log_exec            (const std::string cmd);

#endif // TOOLS_HPP
