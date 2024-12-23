#pragma once

#include <string>
#include <string_view>

#include "om/tiles/tile.hpp"

namespace om::tiles
{
class TileRequestBuilder
{
  enum class Placeholder
  {
    X,
    Y,
    ZoomLevel,
    ApiKey,
    None
  };

  struct UlrPart
  {
    std::string_view m_part;
    Placeholder m_placeholder;
  };

public:
  explicit TileRequestBuilder(std::string url);

  std::string BuildRequestUrl(TileKey tileKey, std::string_view apiKey = "") const;

private:
  static std::vector<UlrPart> PreprocessUrl(std::string_view url);

  const std::string m_url;
  std::vector<UlrPart> const m_urlParts;
};
}  // namespace om::tiles
