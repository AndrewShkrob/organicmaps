#pragma once

#include "om/tiles/types/resolution.hpp"
#include "om/tiles/types/tile_data.hpp"
#include "om/tiles/types/tile_key.hpp"

namespace om::tiles::provider
{
/// @brief OpenStreetMap's Standard tile layer
/// Usage policy: https://operations.osmfoundation.org/policies/tiles/
///
/// Tiles URL: https://tile.openstreetmap.org/{z}/{x}/{y}.png
class OpenStreetMapTileProvider
{
public:
  struct Config
  {
    static constexpr bool kApiKeyRequired = false;
    static constexpr types::ZoomLevel kMinZoomLevel = 0;
    static constexpr types::ZoomLevel kMaxZoomLevel = 19;
    static constexpr types::Resolution kResolution = types::Resolution::Normal;
  };

  [[nodiscard]] std::string GetTileUrl(types::TileKey key) const;
};
}  // namespace om::tiles::provider
