#pragma once

#include <string>

namespace om::tiles
{
struct ServerConfig
{
  enum class Resolution
  {
    Normal,  // 256x256
    High,    // 512x512
  };

  /// Template url in the following format:
  /// https://www.server.com/{z}/{x}/{y}.png?apikey={apiKey}
  /// where:
  /// {z} - zoom level
  /// {x} - tile x
  /// {y} - tile y
  /// {apiKey} - api key
  std::string m_url;
  bool m_apiKeyRequired;
  std::string m_apiKey = "";
  uint8_t m_minZoomLevel;
  uint8_t m_maxZoomLevel;
  Resolution m_resolution;
};
}  // namespace om::tiles
