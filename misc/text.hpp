#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

#include <fmt/core.h>
#include <fmt/format.h>

namespace text {

std::string concatenate(auto &&... args)
{
  std::string result;
  result.reserve((std::string_view(args).size() + ...));
  (result.append(args), ...);
  return result;
}

template <typename T>
constexpr std::string_view hexFmtString()
{
  // Intended for use with fmt::format / std::format
  //
  // The "{}" is a placeholder targeting the vargs supplied to xyz::format
  // ':' ==> formatting expression follows
  // '#' ==> use the following alternative representation
  //     '0' ==> zero pad left
  //     nn  ==> maximum field width, which is calculated to allow for the '0x' prefix and 2 characters per byte
  //     'x' ==> format output as lower-case hex digits

  if constexpr (sizeof(T) == 1) {
    return "{:#04x}";
  }

  if constexpr (sizeof(T) == 2) {
    return "{:#06x}";
  }

  if constexpr (sizeof(T) == 4) {
    return "{:#010x}";
  }

  if constexpr (sizeof(T) == 8) {
    return "{:#018x}";
  }

  // Non-power of two sizes

  if constexpr (sizeof(T) == 3) {
    return "{:#08x}";
  }

  if constexpr (sizeof(T) == 5) {
    return "{:#012x}";
  }

  if constexpr (sizeof(T) == 6) {
    return "{:#014x}";
  }

  if constexpr (sizeof(T) == 7) {
    return "{:#016x}";
  }

  return "{:#0x}";
}

template<typename Integral>
requires std::is_integral_v<Integral>
std::string toHex(Integral const & number)
{
  using UIntegral = std::make_unsigned_t<Integral>;
  constexpr auto fmtString = hexFmtString<UIntegral>();
  return fmt::format(fmtString, static_cast<UIntegral>(number));
}

}
