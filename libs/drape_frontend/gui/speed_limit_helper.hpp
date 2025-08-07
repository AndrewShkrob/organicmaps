#pragma once

#include <string>

namespace gui
{
class SpeedLimitHelper
{
public:
  void SetEnabled(bool enabled) { m_enabled = enabled; }
  void SetSpeedLimit(double speedLimitMps);
  void SetPosition(m2::PointF const & position) { m_position = position; }
  void SetRadius(float radius) { m_radius = radius; }

  bool IsEnabled() const { return m_enabled; }
  bool IsSpeedLimitAvailable() const;
  std::string GetSpeedLimit() const;
  m2::PointF const & GetPosition() const { return m_position; }
  float GetRadius() const { return m_radius; }

private:
  bool m_enabled = true;
  double m_speedLimitMps = 120;
  m2::PointF m_position = {100.0, 100.0};  // Default position, should be updated.
  float m_radius = 40.0f;                  // Default radius, should be updated.
};
}  // namespace gui
