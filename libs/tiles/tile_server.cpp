#include "tile_server.hpp"

#include <utility>

#include "server_thread.hpp"
#include "tile_request_builder.hpp"

namespace om::tiles
{
TileServer::TileServer(ServerConfig config, OnTileReadyCb onTileReadyCb)
  : m_config(std::move(config))
  , m_requestBuilder(m_config.url)
  , m_thread({.config = m_config, .requestBuilder = m_requestBuilder, .tileCallback = std::move(onTileReadyCb)})
{}

void TileServer::RequestTile(TileKey const & key)
{
  m_thread.RequestTile(key);
}
}  // namespace om::tiles
