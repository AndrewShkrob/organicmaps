#pragma once

#include <string>

namespace om::tiles
{
struct ServerConfig
{
  struct Api
  {
    bool keyRequired;
  };

  struct Header
  {
    bool userAgentRequired;
    bool refererRequired;
  };

  struct Attribution
  {
    bool required;
    std::string text;
    std::string url;
  };

  struct Cache
  {
    bool allowed;
    bool ifNoneMatchHeaderSupported;
    bool ifModifiedSinceHeaderSupported;
    uint32_t ttl;
  };

  std::string name;
  std::string url;
  uint8_t minZoom;
  uint8_t maxZoom;
  uint16_t resolution;
  Api api;
  Header header;
  Attribution attribution;
  Cache cache;
};
}  // namespace om::tiles
