#pragma once

#include "tiles/server_config.hpp"
#include "tiles/server_thread.hpp"
#include "tiles/tile.hpp"

namespace om::tiles
{
class TileServer final
{
public:
  using OnTileReadyCb = std::function<void(Tile const &)>;

  explicit TileServer(ServerConfig config, OnTileReadyCb onTileReadyCb);
  void RequestTile(TileKey const & key);

  [[nodiscard]] ServerConfig const & GetConfig() const { return m_config; }

private:
  ServerConfig const m_config;
  TileRequestBuilder const m_requestBuilder;
  internal::ServerThread m_thread;
};
}  // namespace om::tiles
