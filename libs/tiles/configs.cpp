#include "configs.hpp"

#include <glaze/json.hpp>

#include "platform/platform.hpp"

namespace om::tiles::config
{
std::unordered_map<std::string, ServerConfig> GetTileServers()
{
  static std::unordered_map<std::string, ServerConfig> servers;
  if (servers.empty())
  {
    Platform::FilesList files;
    Platform::GetFilesRecursively(GetPlatform().ResourcesDir() + "/tiles/configs", files);
    for (auto const & file : files)
    {
      ServerConfig config;
      if (auto const ec = glz::read_file_json(config, file, std::string{}); ec.ec != glz::error_code::none)
        LOG(LWARNING, ("Failed to load tile server config: ", file, ". ec:", glz::format_error(ec.ec),
                       "custom_error_message:", ec.custom_error_message));
      else
        servers.emplace(config.name, config);
    }
  }
  return servers;
}
}  // namespace om::tiles::config
