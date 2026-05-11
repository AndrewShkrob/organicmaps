#pragma once

#include <cstdint>
#include <vector>

#include "drape_frontend/tile_key.hpp"

namespace om::tiles
{
struct TileKey
{
  uint32_t x;
  uint32_t y;
  uint8_t zoomLevel;

  static TileKey From(df::TileKey const & dfKey)
  {
    return {.x = OmToSlippyX(dfKey.m_x, dfKey.m_zoomLevel),
            .y = OmToSlippyY(dfKey.m_y, dfKey.m_zoomLevel),
            .zoomLevel = dfKey.m_zoomLevel};
  }

  df::TileKey ToDfTileKey() const { return {SlippyToOmX(x, zoomLevel), SlippyToOmY(y, zoomLevel), zoomLevel}; }

private:
  // Center-origin (Organic Maps) → top-left origin (OSM slippy map)
  // om_x range: [-2^(z-1), 2^(z-1))
  // slippy_x range: [0, 2^z)
  static uint32_t OmToSlippyX(int om_x, uint8_t z) { return om_x + (1 << (z - 1)); }
  static uint32_t OmToSlippyY(int om_y, uint8_t z) { return (1 << (z - 1)) - 1 - om_y; }  // Y axis is flipped

  // Top-left origin (OSM slippy map) → center-origin (Organic Maps)
  static int SlippyToOmX(uint32_t x, uint8_t z) { return x - (1 << (z - 1)); }
  static int SlippyToOmY(uint32_t y, uint8_t z) { return (1 << (z - 1)) - 1 - y; }
};

struct Tile
{
  TileKey key;
  std::vector<uint8_t> data;
};
}  // namespace om::tiles
