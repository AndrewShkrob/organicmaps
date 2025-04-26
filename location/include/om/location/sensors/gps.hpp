#pragma once

namespace om::location::sensors
{
struct Gps
{
  double timestamp;
  double latitude;
  double longitude;
  double altitude;
  double horizontalAccuracy;
  double verticalAccuracy;
  double bearing;
  double speed;
};
}  // namespace om::location::sensors
