#pragma once

#include <iostream>

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <variant>
#include <bit>

#include <cassert>

#include "ieee754_types.hpp"

#include "ieee754.h"

namespace IEEE_754 { namespace _2008 {

template <int width>
constexpr auto Layout()
{
  if constexpr(width == 32 && std::endian::native == std::endian::big) {
    struct
    {
      unsigned negative:1;
      unsigned exponent:8;
      unsigned mantissa:23;
    } layout;
    return layout;
  }

  if constexpr(width == 32 && std::endian::native == std::endian::little) {
    struct
    {
      unsigned mantissa:23;
      unsigned exponent:8;
      unsigned negative:1;
    } layout;
    return layout;
  }

  if constexpr(width == 64 && std::endian::native == std::endian::big) {
    struct
    {
      unsigned negative:1;
      unsigned exponent:11;
      uint64_t mantissa:52;
    } layout;
    return layout;
  }

  if constexpr(width == 64 && std::endian::native == std::endian::little) {
    struct
    {
      uint64_t mantissa:52;
      unsigned exponent:11;
      unsigned negative:1;
    } layout;
    return layout;
  }
}

template <int width>
union Encoding
{
  using Float = IEEE_754::_2008::Binary<width>;
  Float value;

  Encoding<width> & operator=(Float const & value) { this->value = value; }

  operator Float() const { return value; }

  using Layout = decltype(IEEE_754::_2008::Layout<width>());
  Layout raw;
};




} } // namespace IEEE_754::_2008

namespace machine {

using NumericTraits = uint32_t;

constexpr NumericTraits SIGNED = 0x00000001;

struct FixedPrecisionTraits
{
  bool isSigned = false;
  int bits = 0;
  int power = 0;
  int minBits = bits;
  int maxBits = 63;
};

template<IEEE_754::_2008::Binary<32> minValue = 0.0f, IEEE_754::_2008::Binary<32> maxValue = 0.0f>
constexpr FixedPrecisionTraits MakeFixedPrecisionTraits()
{
  constexpr auto layoutMin = std::bit_cast<decltype(IEEE_754::_2008::Layout<32>())>(minValue);
  constexpr auto layoutMax = std::bit_cast<decltype(IEEE_754::_2008::Layout<32>())>(maxValue);

  return {.isSigned = (layoutMin.negative | layoutMax.negative) != 0};
}

struct DynamicRange
{
  double min;
  double max;
  double digits;

  constexpr operator FixedPrecisionTraits() {
    return {};
  }
};

template<FixedPrecisionTraits traits>
concept FixedPrecisionTraitsValidator =
    traits.minBits >= 0 && traits.minBits <= traits.bits && traits.bits <= traits.maxBits && traits.maxBits <= 64;

template <FixedPrecisionTraits traits>
requires FixedPrecisionTraitsValidator<traits>
constexpr auto FastestIntegralType()
{
  // Build time type inference

  if constexpr (traits.isSigned && traits.bits > 31 && traits.bits <= 63) {
    return std::int_fast64_t(0);
  }

  if constexpr(!traits.isSigned && traits.bits > 32 && traits.bits <= 64) {
    return std::uint_fast64_t(0);
  }

  if constexpr(traits.isSigned && traits.bits > 15 && traits.bits <= 31) {
    return std::int_fast32_t(0);
  }

  if constexpr(!traits.isSigned && traits.bits > 16 && traits.bits <= 32) {
    return std::uint_fast32_t(0);
  }

  if constexpr(traits.isSigned && traits.bits > 7 && traits.bits <= 15) {
    return std::int_fast16_t(0);
  }

  if constexpr(!traits.isSigned && traits.bits > 8 && traits.bits <= 16) {
    return std::uint_fast16_t(0);
  }

  if constexpr(traits.isSigned && traits.bits <= 7) {
    return std::int_fast8_t(0);
  }

  if constexpr(!traits.isSigned && traits.bits <= 8) {
    return std::uint_fast8_t(0);
  }

  // Build time errors

  static_assert(!traits.isSigned || traits.bits <= 63, "\n\n\33[1;31mError: No SIGNED integral types can store more than 63 data traits.bits!\33[0m\n\n");

  static_assert(traits.isSigned || traits.bits <= 64, "\n\n\33[1;31mError: No UNSIGNED integral types can store more than 64 data traits.bits!\33[0m\n\n");
}

template <NumericTraits traits, int bits>
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

template <NumericTraits traits, int bits>
constexpr auto SmallestIntegralType()
{
  // Build time type inference

  if constexpr ((traits & SIGNED) == SIGNED && bits > 31 && bits <= 63) {
    return std::int_least64_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits > 32 && bits <= 64) {
    return std::uint_least64_t(0);
  }

  if constexpr((traits & SIGNED) == SIGNED && bits > 15 && bits <= 31) {
    return std::int_least32_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits > 16 && bits <= 32) {
    return std::uint_least32_t(0);
  }

  if constexpr((traits & SIGNED) == SIGNED && bits > 7 && bits <= 15) {
    return std::int_least16_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits > 8 && bits <= 16) {
    return std::uint_least16_t(0);
  }

  if constexpr((traits & SIGNED) == SIGNED && bits <= 7) {
    return std::int_least8_t(0);
  }

  if constexpr((traits & SIGNED) != SIGNED && bits <= 8) {
    return std::uint_least8_t(0);
  }

  // Build time errors

  static_assert((traits & SIGNED) != SIGNED || bits <= 63, "\n\n\33[1;31mError: No SIGNED integral types can store more than 63 data bits!\33[0m\n\n");

  static_assert((traits & SIGNED) == SIGNED || bits <= 64, "\n\n\33[1;31mError: No UNSIGNED integral types can store more than 64 data bits!\33[0m\n\n");
}

template<typename IntegralType>
requires std::is_unsigned_v<IntegralType>
int CountLeadingZeroes(IntegralType const number)
{
  if (number == 0) return CHAR_BIT * sizeof(IntegralType);

  if constexpr(sizeof(IntegralType) <= sizeof(unsigned)) {
    return __builtin_clz(number) - CHAR_BIT * (sizeof(unsigned) - sizeof(IntegralType));
  }

  if constexpr(sizeof(IntegralType) > sizeof(unsigned) && sizeof(IntegralType) <= sizeof(unsigned long)) {
    return __builtin_clzl(number) - CHAR_BIT * (sizeof(unsigned long) - sizeof(IntegralType));
  }

  if constexpr(sizeof(IntegralType) > sizeof(unsigned long) && sizeof(IntegralType) <= sizeof(unsigned long long)) {
    return __builtin_clzll(number) - CHAR_BIT * (sizeof(unsigned long long) - sizeof(IntegralType));
  }

  static_assert(sizeof(IntegralType) <= sizeof(unsigned long long), "\n\n\33[1;31mError: No clz builtin for supplied arguement!\33[0m\n\n");
}

template<FixedPrecisionTraits traits>
requires FixedPrecisionTraitsValidator<traits>
struct FixedPrecision
{
public:

  using IntegralType = decltype(FastestIntegralType<traits>());
  IntegralType data = 0;

  static constexpr auto Traits = traits;

  FixedPrecision() = default;
  FixedPrecision(IntegralType const & data) : data(data) {}

  using Float32 = IEEE_754::_2008::Binary<32>;

  operator Float32() const
  {
    // 1. Return zero for the trivial case

    if (data == 0) {
      return 0.0;
    }


    constexpr int MantissaBits = 23;
    constexpr int ExponentialMax = 127;
    constexpr int ExponentialBias = 127;
    constexpr int ExponentialMin = -127;

    // 2. Check the theoretical data bits,
    //    Return +inf, -inf, or zero if impossible to represent in the return type

    constexpr auto MSB = traits.bits + traits.power;
    constexpr auto LSB = 1 + traits.power;

    bool const isNegative = traits.isSigned && data < 0;

    if constexpr(MSB > ExponentialMax) {
      if (isNegative) {
        return -std::numeric_limits<Float32>::infinity();
      } else {
        return std::numeric_limits<Float32>::infinity();
      }
    }

    if constexpr(LSB < ExponentialMin) {
      return  0.0;
    }

    // 3. Check the actual data bits,
    //    Return +inf, -inf if impossible to represent in the return type

    typename std::make_unsigned<IntegralType>::type digits;

    if constexpr(traits.isSigned) {
      digits = std::abs(data);
    } else {
      digits = data;
    }

    std::cout << "Here" << std::endl;

    auto const leadingZeroes = CountLeadingZeroes(digits);
    auto const actualDataBits = (CHAR_BIT * sizeof(IntegralType)) - leadingZeroes;
    auto const msb = actualDataBits + traits.power;

    if (msb > ExponentialMax) {
      if (isNegative) {
        return -std::numeric_limits<Float32>::infinity();
      } else {
        return std::numeric_limits<Float32>::infinity();
      }
    }
    std::cout << "Here2" << std::endl;

    // 4. Manually initialize the return type

    decltype(IEEE_754::_2008::Layout<32>()) raw;

    raw.negative = isNegative ? 1 : 0;
    raw.mantissa = digits << (MantissaBits - actualDataBits + 1);

    std::cout << "digits " << digits << std::endl;
    std::cout << "traits.power " << traits.power << std::endl;
    std::cout << "actualDataBits " << actualDataBits << std::endl;
    std::cout << "leadingZeroes  " << leadingZeroes << std::endl;

    raw.exponent = ExponentialBias + msb - 1;

    std::cout << "raw.negative " << raw.negative << std::endl;
    std::cout << std::hex;
    std::cout << "raw.mantissa 0x" << raw.mantissa << std::endl;
    std::cout << "raw.exponent 0x" << raw.exponent << std::endl;
    std::cout << std::dec;

    Float32 value = std::bit_cast<Float32>(raw);
    
  std::cout << "value = " << std::setprecision(15) << value << std::endl;

   if (traits.power >= 0) {
     value = (float) data * (1 << traits.power);
   } else {
     value = (float) data / (1 << std::abs(traits.power));
   }

   raw = std::bit_cast<decltype(raw)>(value);

   std::cout << "raw.negative " << raw.negative << std::endl;
   std::cout << std::hex;
   std::cout << "raw.mantissa 0x" << raw.mantissa << std::endl;
   std::cout << "raw.exponent 0x" << raw.exponent << std::endl;
   std::cout << std::dec;

   value = std::bit_cast<Float32>(raw);
   

  std::cout << "value = " << std::setprecision(15) << value << std::endl;
 
    return value;
  }



  template <FixedPrecisionTraits multiplicandTraits, FixedPrecisionTraits multiplierTraits>
  friend decltype(auto) operator*(FixedPrecision<multiplicandTraits> multiplicand, FixedPrecision<multiplierTraits> multiplier);

  template <FixedPrecisionTraits dividendTraits, FixedPrecisionTraits divisorTraits>
  friend decltype(auto) operator/(FixedPrecision<dividendTraits> dividend, FixedPrecision<divisorTraits> rhs);
};

//template <
//constexpr FixedPrecisionTraits makeTraits<

template <FixedPrecisionTraits multiplicandTraits, FixedPrecisionTraits multiplierTraits>
decltype(auto) operator*(FixedPrecision<multiplicandTraits> muliplicand, FixedPrecision<multiplierTraits> multiplier)
{
  constexpr auto maxBits = std::min(multiplicandTraits.maxBits, multiplierTraits.maxBits);
  constexpr auto minBits = std::min(maxBits, multiplicandTraits.minBits + multiplierTraits.minBits);
  //constexpr auto minProductBits = std::max(multiplicandTraits.bits, multiplierTraits.bits);
  constexpr auto maxProductBits = multiplicandTraits.bits + multiplierTraits.bits;
  constexpr auto productPower = multiplicandTraits.power + multiplierTraits.power;

  constexpr FixedPrecisionTraits productTraits = {
    .isSigned = multiplicandTraits.isSigned || multiplierTraits.isSigned,
    .bits = maxProductBits < maxBits ? maxProductBits : maxBits,
    .power = maxProductBits < maxBits ? productPower : productPower + maxProductBits - maxBits,
    .minBits = minBits,
    .maxBits = maxBits};

  return FixedPrecision<productTraits>(static_cast<decltype(FastestIntegralType<productTraits>())>(muliplicand.data) * multiplier.data);
}

// NB. The division operators are wrong
// Need to figure out how to compensate for:
// - Divisors that are bigger than the dividend
// - The number of bits to assign to the quotient.
//   For integral quotients this is simply bits(dividend) - bits(divisor).
//   But for the general case this is unbounded.
//   My current thoughts are that the dividend could be bit shifted left with a complementary change made to the power.
//   This would prevent the loss of bits due to the division.
//   There are several options:
//   - Fill the containing fastest_integral container.
//   - Shift to preserve the relationship, bits(divisor) = bits(quotient) + bits(dividend).
//     Knowing that at least one bit is needed in the quotient.
//   I need to figure out how to use / calculate the isSigned, bits, minBits, and maxBits traits in this process.

template <FixedPrecisionTraits dividendTraits, FixedPrecisionTraits divisorTraits>
decltype(auto) operator/(FixedPrecision<dividendTraits> dividend, FixedPrecision<divisorTraits> divisor)
{
  constexpr auto maxBits = std::min(dividendTraits.maxBits, divisorTraits.maxBits);
  constexpr auto minBits = std::min(maxBits, dividendTraits.minBits + divisorTraits.minBits);
  //constexpr auto minQuotientBits = dividendTraits.bits - divisorTraits.bits;
  //constexpr auto maxQuotientBits = dividendTraits.bits;
  constexpr auto quotientPower = dividendTraits.power - divisorTraits.power;

  // Since integral division is continuous for all divisors != 0,
  // spread the result across as many bits as possible and adjust the power of the quotient accordingly

  constexpr FixedPrecisionTraits quotientTraits = {
    .isSigned = dividendTraits.isSigned || divisorTraits.isSigned,
    .bits = maxBits,
    .power = quotientPower + dividendTraits.bits - maxBits,
    .minBits = minBits,
    .maxBits = maxBits};

  // NB. The order and kind of operations performed during the actual division are critical,
  //     and depend on whether the quotient requires an integral promotion or demotion.
  //
  //     For situations requiring no change or promotion of the quotient's underlying integral,
  //     a static type cast to that integral must be performed prior to any left-shifts on the dividend.
  //
  //     This includes the cases of no underlying promotion or a shift of zero.
  //
  //     For situations requiring a demotion of the quotient's underlying integral,
  //     right-shifts must be performed prior to static type casts of the dividend and the divisor.
  //

  if constexpr (dividendTraits.bits <= maxBits) {
    return FixedPrecision<quotientTraits>(
        (                                                                                 // Adjust dividend
           static_cast<decltype(FastestIntegralType<quotientTraits>())>(dividend.data)    //  (1) Promote if needed
           << (maxBits - dividendTraits.bits)                                             //  (2) Fill underlying integral type
        )
        / divisor.data);                                                                  // Perform division
  } else {
    return FixedPrecision<quotientTraits>(
        (                                                                                 // Adjust dividend
           static_cast<decltype(FastestIntegralType<quotientTraits>())>                   //  (2) Demote if needed
           (dividend.data >> (maxBits - dividendTraits.bits))                             //  (1) Discard overflow bits
        )
        / static_cast<decltype(FastestIntegralType<quotientTraits>())>(divisor.data));    // Demote divisor if needed then perform division
  }
}

//template <FixedPrecisionTraits traits_a, int bits_a, int power_a, FixedPrecisionTraits traits_b, int bits_b, int power_b>
//decltype(auto) operator*(FixedPrecision<traits_a, bits_a, power_a> && lhs, FixedPrecision<traits_b, bits_b, power_b> && rhs)
//{
//  return FixedPrecision<traits_a | traits_b, bits_a + bits_b, power_a + power_b>(
//    static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) * rhs.data);
//}
//
//template <FixedPrecisionTraits traits_a, int bits_a, int power_a, FixedPrecisionTraits traits_b, int bits_b, int power_b>
//decltype(auto) operator/(FixedPrecision<traits_a, bits_a, power_a> const & lhs, FixedPrecision<traits_b, bits_b, power_b> const & rhs)
//{
//  return FixedPrecision<traits_a | traits_b, bits_a, power_a - power_b - bits_b>(
//    (static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) << bits_b) / rhs.data);
//}
//
//template <FixedPrecisionTraits traits_a, int bits_a, int power_a, FixedPrecisionTraits traits_b, int bits_b, int power_b>
//decltype(auto) operator/(FixedPrecision<traits_a, bits_a, power_a> && lhs, FixedPrecision<traits_b, bits_b, power_b> && rhs)
//{
//  return FixedPrecision<traits_a | traits_b, bits_a, power_a - power_b - bits_b>(
//    (static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) << bits_b) / rhs.data);
//}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


template <NumericTraits traits, int bits, int power>
class FixedPoint
{
private:
  using IntegralType = decltype(FastestIntegralType<traits, bits>());
  IntegralType data = 0;

public:
  static constexpr auto Bits = bits;
  static constexpr auto Power = power;

  using QNumberType = decltype(SmallestIntegralType<traits, bits + (power > 0 ? power : 0)>());

  QNumberType getQNumber() const { return power <= 0 ? data : data << power; }

  // Implicit QNumberType Conversion
  operator QNumberType() const { return getQNumber(); }

  static constexpr int NumberIntegralBits() { return bits + power; }

  static constexpr int HasIntegralPart() { return NumberIntegralBits() > 0; }

  auto getIntegralPart() const {
    if constexpr (NumberIntegralBits() > 0) {
      using ResultType = decltype(SmallestIntegralType<traits, NumberIntegralBits()>());

      ResultType result;

      if (power == 0) {
        result = data;
      } else if (power > 0) {
        result = static_cast<ResultType>(data) << power;
      } else {
        result = static_cast<ResultType>(data) >> -power;
      }

      return result;
    } else {
      return 0;
    }
  }

  static constexpr int NumberFractionalBits() { return bits - (bits + power); }

  static constexpr int HasFractionalPart() { return NumberFractionalBits() > 0; }

  auto getFractionalPart() const {
    if constexpr (NumberFractionalBits() > 0) {
      using ResultType = decltype(SmallestIntegralType<traits, NumberFractionalBits()>());

      ResultType result = ((1 << NumberFractionalBits()) - 1) & data;

      if (data < 0 && NumberIntegralBits() <= 0) {
        result *= -1;
      }

      return result;
    } else {
      return 0;
    }
  }

  FixedPoint() = default;
  FixedPoint(IntegralType const & data) : data(data) {}

  template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
  friend decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> const & lhs, FixedPoint<traits_b, bits_b, power_b> const & rhs);

  template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
  friend decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> && lhs, FixedPoint<traits_b, bits_b, power_b> && rhs);

  template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
  friend decltype(auto) operator/(FixedPoint<traits_a, bits_a, power_a> const & lhs, FixedPoint<traits_b, bits_b, power_b> const & rhs);

  template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
  friend decltype(auto) operator/(FixedPoint<traits_a, bits_a, power_a> && lhs, FixedPoint<traits_b, bits_b, power_b> && rhs);

};

template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> const & lhs, FixedPoint<traits_b, bits_b, power_b> const & rhs)
{
  return FixedPoint<traits_a | traits_b, bits_a + bits_b, power_a + power_b>(
    static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) * rhs.data);
}

template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
decltype(auto) operator*(FixedPoint<traits_a, bits_a, power_a> && lhs, FixedPoint<traits_b, bits_b, power_b> && rhs)
{
  return FixedPoint<traits_a | traits_b, bits_a + bits_b, power_a + power_b>(
    static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) * rhs.data);
}

template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
decltype(auto) operator/(FixedPoint<traits_a, bits_a, power_a> const & lhs, FixedPoint<traits_b, bits_b, power_b> const & rhs)
{
  return FixedPoint<traits_a | traits_b, bits_a, power_a - power_b - bits_b>(
    (static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) << bits_b) / rhs.data);
}

template <NumericTraits traits_a, int bits_a, int power_a, NumericTraits traits_b, int bits_b, int power_b>
decltype(auto) operator/(FixedPoint<traits_a, bits_a, power_a> && lhs, FixedPoint<traits_b, bits_b, power_b> && rhs)
{
  return FixedPoint<traits_a | traits_b, bits_a, power_a - power_b - bits_b>(
    (static_cast<decltype(FastestIntegralType<traits_a | traits_b, bits_a + bits_b>())>(lhs.data) << bits_b) / rhs.data);
}

}; // namespace machine

