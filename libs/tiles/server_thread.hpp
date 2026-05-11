#pragma once

#include "base/thread.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>

#include "cache.hpp"
#include "platform/http_client.hpp"

#include "tile_request_builder.hpp"

namespace om::tiles::internal
{
class ServerThread
{
public:
  using TileCallbackFn = std::function<void(Tile const &)>;

  struct Config
  {
    ServerConfig const & config;
    TileRequestBuilder const & requestBuilder;
    TileCallbackFn tileCallback;
  };

  explicit ServerThread(Config const & config)
    : m_requestBuilder(config.requestBuilder)
    , m_tileCallback(config.tileCallback)
    , m_thread(&ServerThread::ThreadRoutine, this)
    , m_config(config.config)
    , m_cache(m_config)
  {}

  ~ServerThread()
  {
    {
      std::lock_guard lock(m_tilesQueueMutex);
      m_isRunning = false;
    }
    m_cv.notify_one();
    m_thread.join();
  }

  void RequestTile(TileKey const & key)
  {
    {
      std::lock_guard lock(m_tilesQueueMutex);
      m_requestedTiles.push(key);
    }
    m_cv.notify_one();
  }

private:
  void ThreadRoutine()
  {
    while (true)
    {
      TileKey key;
      {
        std::unique_lock lock(m_tilesQueueMutex);
        // Block until there is work or we are asked to stop.
        m_cv.wait(lock, [this] { return !m_requestedTiles.empty() || !m_isRunning; });

        if (!m_isRunning && m_requestedTiles.empty())
          return;

        key = m_requestedTiles.front();
        m_requestedTiles.pop();
      }  // release lock before the blocking HTTP call

      if (m_cache.IsCached(key))
      {
        m_tileCallback({.key = key, .data = m_cache.LoadTile(key)});
        continue;
      }

      LOG(LINFO, ("Requesting tile from server:", key.x, key.y, static_cast<int>(key.zoomLevel)));
      std::string const tileUrl = m_requestBuilder.BuildRequestUrl(key);
      LOG(LDEBUG, ("Request URL:", tileUrl));
      platform::HttpClient request(m_requestBuilder.BuildRequestUrl(key));
      request.LoadHeaders(true);
      if (m_config.header.userAgentRequired)
        request.SetRawHeader("User-Agent", "OrganicMaps/1.0 (test@organicmaps.app)");

      request.RunHttpRequestAsync([this, key](platform::HttpClient::Result && result)
      { OnImageDownloaded(key, std::forward<platform::HttpClient::Result>(result)); });
    }
  }

  void OnImageDownloaded(TileKey const & key, platform::HttpClient::Result result)
  {
    if (result.m_success)
    {
      std::vector<uint8_t> const data(result.m_serverResponse.begin(), result.m_serverResponse.end());

      m_cache.SaveTile(key, data);
      m_tileCallback({.key = key, .data = m_cache.LoadTile(key)});
    }
    else
    {
      // RequestTile(key);  // re-queue the tile for download; could add retry limit if needed
    }
  }

  bool m_isRunning{true};  // guarded by m_tilesQueueMutex

  std::mutex m_tilesQueueMutex;
  std::condition_variable m_cv;
  std::queue<TileKey> m_requestedTiles;

  TileRequestBuilder const & m_requestBuilder;
  TileCallbackFn m_tileCallback;

  threads::SimpleThread m_thread;  // declared last so it starts after all members are ready

  ServerConfig const & m_config;
  Cache m_cache;
};
}  // namespace om::tiles::internal
