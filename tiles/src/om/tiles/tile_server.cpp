#include "om/tiles/tile_server.hpp"

#include "server_thread.hpp"

namespace om::tiles
{
class TileServer::Impl
{
public:
  explicit Impl(ServerConfig const & config)
    : m_config(config)
    , m_requestBuilder(config.m_url)
    , m_thread({.requestBuilder = m_requestBuilder, .tileCallback = nullptr})
  {
  }

  // blocking
  Tile GetTile(TileKey const & key) { return {.key = key, .data = {}}; }
  std::vector<Tile> GetTiles(std::vector<TileKey> const & keys) { return {}; }

  // non-blocking
  void RequestTile(TileKey const & key) const {}
  void RequestTiles(std::vector<TileKey> const & keys) const {}

private:
  ServerConfig const m_config;
  TileRequestBuilder const m_requestBuilder;
  internal::ServerThread m_thread;
};

TileServer::TileServer(ServerConfig const & config) : m_impl(std::make_unique<Impl>(config)) {}

TileServer::~TileServer() = default;

Tile TileServer::GetTile(const om::tiles::TileKey & key) { return m_impl->GetTile(key); }
}  // namespace om::tiles