#ifndef RSYNC_TO_HPP
#define RSYNC_TO_HPP

#include <json/json.h>

namespace ope {

int rsync_to_local (Json::Value & ope);
int rsync_to_remote (Json::Value & ope);

} // namespace ope

#endif // RSYNC_TO_HPP
