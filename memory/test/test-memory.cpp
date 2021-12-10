#include <string>
#include <iostream>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <string_view>

#include <memory>
#include <cstddef>
#include <array>

#include <fmt/core.h>
#include <fmt/format.h>

#include "memory/small-pool.hpp"

using namespace culyun::memory;

int main()
{
  fmt::print("Hi there {}\n", "fred");

  fmt::print("std::max_align_t = {}\n",  alignof(std::max_align_t));

  std::byte buffer[16 * 1024] = {};

  SmallPool sp(buffer, sizeof(buffer));

  std::unique_ptr<Waypoint> wp = std::make_unique<Waypoint>();

  fmt::print("Leaving..");

  return 0;
}
