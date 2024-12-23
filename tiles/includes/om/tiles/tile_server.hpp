#pragma once

#include <memory>
#include <string>

#include "om/tiles/server_config.hpp"
#include "om/tiles/tile.hpp"

namespace om::tiles
{
class TileServer final
{
public:
  explicit TileServer(ServerConfig const & config);
  ~TileServer();

  Tile GetTile(TileKey const & key);

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};
}  // namespace om::tiles