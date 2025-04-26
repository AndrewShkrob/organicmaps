#include "om/tiles/provider/osm.hpp"

namespace om::tiles::provider
{
std::string OpenStreetMapTileProvider::GetTileUrl(types::TileKey const key) const
{
  return "https://tile.openstreetmap.org/" + std::to_string(key.z) + "/" + std::to_string(key.x) + "/" +
         std::to_string(key.y) + ".png";
}
}  // namespace om::tiles::provider
