#pragma once

#include "geometry/screenbase.hpp"

namespace om::tiles::server
{
class Server
{
public:
  Server();
  ~Server();

  void UpdateViewport(ScreenBase const & screen);

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};
}  // namespace om::tiles::server
