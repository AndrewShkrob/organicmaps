#pragma once

#include <cstdint>
#include <vector>

namespace om::tiles
{
struct TileKey
{
  uint32_t x;
  uint32_t y;
  uint8_t zoomLevel;
};

struct Tile
{
  TileKey key;
  std::vector<uint8_t> data;
};
}  // namespace om::tiles
