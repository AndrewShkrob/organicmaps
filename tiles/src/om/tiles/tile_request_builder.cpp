#include "om/tiles/tile_request_builder.hpp"

namespace om::tiles
{
TileRequestBuilder::TileRequestBuilder(std::string url) : m_url(std::move(url)), m_urlParts(PreprocessUrl(m_url)) {}

std::string TileRequestBuilder::BuildRequestUrl(TileKey tileKey, std::string_view apiKey) const
{
  std::string request;
  for (auto const & part : m_urlParts)
  {
    request += part.m_part;
    switch (part.m_placeholder)
    {
    case Placeholder::X: request += std::to_string(tileKey.x); break;
    case Placeholder::Y: request += std::to_string(tileKey.y); break;
    case Placeholder::ZoomLevel: request += std::to_string(tileKey.zoomLevel); break;
    case Placeholder::ApiKey: request += apiKey; break;
    case Placeholder::None: break;
    }
  }
  return request;
}

std::vector<TileRequestBuilder::UlrPart> TileRequestBuilder::PreprocessUrl(std::string_view url)
{
  std::vector<TileRequestBuilder::UlrPart> urlParts;
  size_t pos = 0;
  while (pos < url.size())
  {
    size_t openBrace = url.find('{', pos);
    size_t closeBrace = url.find('}', openBrace);
    if (openBrace == std::string::npos || closeBrace == std::string::npos)
    {
      urlParts.push_back({url.substr(pos), Placeholder::None});
      break;
    }

    Placeholder placeholder;
    std::string_view key = url.substr(openBrace + 1, closeBrace - openBrace - 1);
    if (key == "x")
      placeholder = Placeholder::X;
    else if (key == "y")
      placeholder = Placeholder::Y;
    else if (key == "z")
      placeholder = Placeholder::ZoomLevel;
    else if (key == "apiKey")
      placeholder = Placeholder::ApiKey;
    else
      placeholder = Placeholder::None;

    urlParts.push_back({url.substr(pos, openBrace - pos), placeholder});

    pos = closeBrace + 1;
  }
  return urlParts;
}
}  // namespace om::tiles
