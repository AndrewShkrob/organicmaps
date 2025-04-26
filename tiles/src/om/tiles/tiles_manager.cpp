#include "om/tiles/tiles_manager.hpp"

#include "om/tiles/server/server.hpp"

namespace om::tiles
{
class TilesManager::Impl
{
public:
  void UpdateViewport(ScreenBase const & screen)
  {
    m_server.UpdateViewport(screen);
  }

private:
  server::Server m_server;
};

TilesManager::TilesManager()
  : m_impl(std::make_unique<Impl>())
{}

TilesManager::~TilesManager() = default;

void TilesManager::UpdateViewport(ScreenBase const & screen) { m_impl->UpdateViewport(screen); }
}  // namespace om::tiles
