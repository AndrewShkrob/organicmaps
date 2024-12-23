#pragma once

#include <unordered_map>

#include "om/tiles/server_config.hpp"

namespace om::tiles::config
{
ServerConfig const kOSM = {.m_url = "https://tile.openstreetmap.org/{z}/{x}/{y}.png",
                           .m_apiKeyRequired = false,
                           .m_minZoomLevel = 0,
                           .m_maxZoomLevel = 19,
                           .m_resolution = ServerConfig::Resolution::Normal};
ServerConfig const kOPNVKarte = {.m_url = "https://tile.memomaps.de/tilegen/{z}/{x}/{y}.png",
                                 .m_apiKeyRequired = false,
                                 .m_minZoomLevel = 0,
                                 .m_maxZoomLevel = 18,
                                 .m_resolution = ServerConfig::Resolution::Normal};

std::unordered_map<std::string, ServerConfig> GetTileServers()
{
  static std::unordered_map<std::string, ServerConfig> servers = {{"OpenStreetMap", kOSM}, {"ÖPNVKarte", kOPNVKarte}};
  return servers;
}
}  // namespace om::tiles::config
