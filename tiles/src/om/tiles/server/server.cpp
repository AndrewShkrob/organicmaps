#include "om/tiles/server/server.hpp"

#include "3party/gflags/src/util.h"
#include "base/thread.hpp"
#include "drape_frontend/visual_params.hpp"
#include "om/tiles/utils.hpp"

namespace
{
constexpr auto kUpdateInterval = std::chrono::minutes(1);

struct ViewportInfo
{
  m2::RectD viewport;
  om::tiles::types::ZoomLevel zoomLevel;
};
}  // namespace

namespace om::tiles::server
{
class Server::Impl
{
public:
  Impl()
    : m_thread(&Impl::ThreadRoutine, this)
  {}

  bool IsEnabled() const { return m_isRunning; }

  void UpdateViewport(ScreenBase const & screen)
  {
    if (!IsEnabled())
      return;

    std::lock_guard lock(m_mutex);
    m_requestedData.emplace(screen.ClipRect(), df::GetZoomLevel(screen.GetScale()));
    m_condition.notify_one();
  }

  void ThreadRoutine()
  {
    while (WaitForRequest())
    {

    }
  }

  bool WaitForRequest()
  {
    std::unique_lock lock(m_mutex);

    bool const timeout =
      !m_condition.wait_for(lock, kUpdateInterval, [this] { return !m_isRunning || m_requestedData; });

    if (!IsEnabled())
      return false;

    if (timeout)
      return false;

    if (!m_requestedData)
      return true;  // TODO
  }

private:
  std::optional<ViewportInfo> m_requestedData;

  bool m_isRunning = false;
  std::condition_variable m_condition;
  std::mutex m_mutex;
  threads::SimpleThread m_thread;
};

Server::Server()
  : m_impl(std::make_unique<Impl>())
{}

Server::~Server() = default;

void Server::UpdateViewport(ScreenBase const & screen) { m_impl->UpdateViewport(screen); }
}  // namespace om::tiles::server
