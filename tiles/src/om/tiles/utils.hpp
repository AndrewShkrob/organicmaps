#pragma once

#include "geometry/mercator.hpp"
#include "om/tiles/types/tile_key.hpp"

namespace om::tiles
{
inline std::vector<types::TileKey> GetTilesForRect(m2::RectD const & bounding_box, types::ZoomLevel const zoom)
{
  int const tilesPerEdge = 1 << zoom;
  double const tileWidth = mercator::Bounds::kRangeX / tilesPerEdge;
  double const tileHeight = mercator::Bounds::kRangeY / tilesPerEdge;

  auto const xStart =
    base::Clamp<std::uint32_t>((bounding_box.minX() - mercator::Bounds::kMinX) / tileWidth, 0, tilesPerEdge - 1);
  auto const xEnd =
    base::Clamp<std::uint32_t>((bounding_box.maxX() - mercator::Bounds::kMinX) / tileWidth, 0, tilesPerEdge - 1);
  auto const yStart =
    base::Clamp<std::uint32_t>((mercator::Bounds::kMaxY - bounding_box.maxY()) / tileHeight, 0, tilesPerEdge - 1);
  auto const yEnd =
    base::Clamp<std::uint32_t>((mercator::Bounds::kMaxY - bounding_box.minY()) / tileHeight, 0, tilesPerEdge - 1);

  std::vector<types::TileKey> result;
  for (std::uint32_t x = xStart; x <= xEnd; ++x)
    for (std::uint32_t y = yStart; y <= yEnd; ++y)
      result.emplace_back(x, y, zoom);

  return result;
}
}  // namespace om::tiles
