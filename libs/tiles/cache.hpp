#pragma once

#include <filesystem>
#include <string>

#include "3party/stb_image/stb_image.h"
#include "base/file_name_utils.hpp"
#include "coding/file_reader.hpp"
#include "coding/file_writer.hpp"
#include "platform/platform.hpp"
#include "server_config.hpp"
#include "tile.hpp"

namespace om::tiles
{
class Cache
{
public:
  explicit Cache(ServerConfig const & config)
    : m_config(config)
    , m_cacheDirName(base::JoinPath("tiles", "cache", m_config.name))
  {}

  [[nodiscard]] bool IsCached(TileKey const & key) const
  {
    return Platform::IsFileExistsByFullPath(GetTileAbsolutePath(key));
  }

  [[nodiscard]] std::vector<uint8_t> LoadTile(TileKey const & key) const
  {
    std::string const tilePath = GetTilePath(key);
    auto const reader = GetPlatform().GetReader(tilePath);
    uint64_t const size = reader->Size();
    std::vector<uint8_t> buffer(size);
    reader->Read(0, buffer.data(), size);

    int width, height, channels;
    uint8_t * data = stbi_load_from_memory(buffer.data(), static_cast<int>(buffer.size()), &width, &height, &channels,
                                           STBI_rgb_alpha);
    std::vector<uint8_t> rgbaBuffer(width * height * 4);
    std::copy_n(data, width * height * 4, rgbaBuffer.begin());
    stbi_image_free(data);

    return rgbaBuffer;
  }

  void SaveTile(TileKey const & tileKey, std::vector<uint8_t> const & image) const
  {
    std::string const tilePath = GetTilePath(tileKey);
    std::filesystem::path const absolutePath = GetTileAbsolutePath(tileKey);
    if (!std::filesystem::exists(absolutePath))
    {
      std::error_code ec;
      std::filesystem::create_directories(absolutePath.parent_path(), ec);
      if (ec)
      {
        LOG(LERROR, ("Failed to create cache directory for tile", tilePath, ec.message()));
        return;
      }
      FileWriter writer(absolutePath.c_str());
      writer.Write(image.data(), image.size());
    }
  }

private:
  [[nodiscard]] std::string GetTileAbsolutePath(TileKey const & key) const
  {
    return base::JoinPath(GetPlatform().WritableDir(), GetTilePath(key));
  }

  [[nodiscard]] std::string GetTilePath(TileKey const & key) const
  {
    return base::JoinPath(m_cacheDirName, std::to_string(key.zoomLevel), std::to_string(key.x),
                          std::to_string(key.y) + ".tile");
  }

  ServerConfig const & m_config;
  std::string m_cacheDirName;
};
}  // namespace om::tiles
