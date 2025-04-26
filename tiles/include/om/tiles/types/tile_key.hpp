#pragma once

#include <cstdint>

namespace om::tiles::types
{
using ZoomLevel = std::uint8_t;

struct TileKey
{
  std::uint32_t x;
  std::uint32_t y;
  ZoomLevel z;
};
}  // namespace om::tiles::types
