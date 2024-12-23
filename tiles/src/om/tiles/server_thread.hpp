#pragma once

#include "base/thread.hpp"

#include <vector>

namespace om::tiles::internal
{
class ServerThread
{
public:
  using TileCallbackFn = std::function<void(Tile const &)>;

  struct Config
  {
    TileRequestBuilder const & requestBuilder;
    TileCallbackFn tileCallback;
  };

  explicit ServerThread(Config config)
    : m_thread(&ServerThread::ThreadRoutine, this), m_requestBuilder(config.requestBuilder)
  {
  }

private:
  bool WaitForRequest() { return true; }

  void ThreadRoutine()
  {
    while (m_isRunning)
    {
      if (!m_requestedTiles.empty())
      {

      }
    }
  }

  std::atomic_bool m_isRunning{true};

  std::vector<TileKey> m_requestedTiles;
  threads::SimpleThread m_thread;

  TileRequestBuilder const & m_requestBuilder;
  TileCallbackFn m_tileCallback;
};
}  // namespace om::tiles::internal
