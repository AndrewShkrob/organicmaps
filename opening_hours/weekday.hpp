#pragma once

#include <string>
#include <cstdint>

namespace om::opening_hours
{
enum class Weekday : std::uint8_t
{
  Monday = 0,
  Tuesday,
  Wednesday,
  Thursday,
  Friday,
  Saturday,
  Sunday,
  Invalid = 255
};

std::string DebugPrint(Weekday weekday);
}  // namespace om::opening_hours
