#pragma once

#include <memory>

class ScreenBase;

namespace om::tiles
{
class TilesManager
{
public:
  TilesManager();
  ~TilesManager();

  void UpdateViewport(ScreenBase const & screen);

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};
}  // namespace om::tiles
