#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

#include <cassert>

namespace machine {

using NumericTraits = uint32_t;

constexpr NumericTraits SIGNED = 0x00000001;

template <NumericTraits traits, unsigned bits>
constexpr auto FastestIntegralType()
{
  // Build time type inference

  if constexpr ((traits & SIGNED) == SIGNED && bits > 31 && bits <= 63) {
    return std::int_fast64_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits > 32 && bits <= 64) {
    return std::uint_fast64_t(0);
  }

  if constexpr((traits & SIGNED) == SIGNED && bits > 15 && bits <= 31) {
    return std::int_fast32_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits > 16 && bits <= 32) {
    return std::uint_fast32_t(0);
  }

  if constexpr((traits & SIGNED) == SIGNED && bits > 7 && bits <= 15) {
    return std::int_fast16_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits > 8 && bits <= 16) {
    return std::uint_fast16_t(0);
  }

  if constexpr((traits & SIGNED) == SIGNED && bits <= 7) {
    return std::int_fast8_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits <= 8) {
    return std::uint_fast8_t(0);
  }

  // Build time errors

  static_assert((traits & SIGNED) != SIGNED || bits <= 63, "\n\n\33[1;31mError: No SIGNED integral types can store more than 63 data bits!\33[0m\n\n");

  static_assert((traits & SIGNED) == SIGNED || bits <= 64, "\n\n\33[1;31mError: No UNSIGNED integral types can store more than 64 data bits!\33[0m\n\n");
}

template <NumericTraits traits, unsigned bits, int power>
class FixedPoint
{
private:
  using IntegralType = decltype(FastestIntegralType<traits, bits>());
  IntegralType data = 0;

public:
  static constexpr auto Bits = bits;
  static constexpr auto Power = power;


  // Implicit IntegralType Conversion
  //operator IntegralType() const { return power == 0 ? data : power > 0 ? data << power : data >> -power; }
  //operator IntegralType() const { return data; }

  IntegralType getData() const { return data; }

  FixedPoint<traits, bits, power>() = default;
  FixedPoint<traits, bits, power>(IntegralType const & data) : data(data) {}

  template <NumericTraits traits_a, unsigned bits_a, int power_a, NumericTraits traits_b, unsigned bits_b, int power_b>
  friend decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> const & lhs, FixedPoint<traits_b, bits_b, power_b> const & rhs);

  template <NumericTraits traits_a, unsigned bits_a, int power_a, NumericTraits traits_b, unsigned bits_b, int power_b>
  friend decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> && lhs, FixedPoint<traits_b, bits_b, power_b> && rhs);

};

template <NumericTraits traits_a, unsigned bits_a, int power_a, NumericTraits traits_b, unsigned bits_b, int power_b>
decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> const & lhs, FixedPoint<traits_b, bits_b, power_b> const & rhs)
{
    return FixedPoint<traits_a | traits_b, bits_a + bits_b, power_a + power_b>(
        static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) * rhs.data);
}

template <NumericTraits traits_a, unsigned bits_a, int power_a, NumericTraits traits_b, unsigned bits_b, int power_b>
decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> && lhs, FixedPoint<traits_b, bits_b, power_b> && rhs)
{
    return FixedPoint<traits_a | traits_b, bits_a + bits_b, power_a + power_b>(
        static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) * rhs.data);
}

}; // namespace machine

