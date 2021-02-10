#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace machine {

using NumericTraits = uint32_t;

constexpr NumericTraits SIGNED = 0x00000001;

template <NumericTraits traits, unsigned bits>
constexpr auto FastestIntegralType()
{
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
  if constexpr (bits_a > bits_b) {
    return FixedPoint<traits_a, bits_a, power_a>(lhs.data * rhs.data);
  } else {
    return FixedPoint<traits_b, bits_b, power_b>(lhs.data * rhs.data);
  }
}

template <NumericTraits traits_a, unsigned bits_a, int power_a, NumericTraits traits_b, unsigned bits_b, int power_b>
decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> && lhs, FixedPoint<traits_b, bits_b, power_b> && rhs)
{
  if constexpr (bits_a > bits_b) {
    return FixedPoint<traits_a, bits_a, power_a>(lhs.data * rhs.data);
  } else {
    return FixedPoint<traits_b, bits_b, power_b>(lhs.data * rhs.data);
  }
}

}; // namespace machine

