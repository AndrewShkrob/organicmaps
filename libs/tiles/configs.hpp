#pragma once

#include <unordered_map>

#include "tiles/server_config.hpp"

namespace om::tiles::config
{
std::unordered_map<std::string, ServerConfig> GetTileServers();
}  // namespace om::tiles::config
