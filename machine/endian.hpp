#pragma once

#include <bit>
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <concepts>
#include <utility>

#include <iostream>

#include <tao/operators.hpp>

namespace culyun { namespace endian {

template<typename StorageType>
concept EndianIntegral =
    std::is_integral_v<StorageType> && (sizeof(StorageType) == 2 || sizeof(StorageType) == 4 || sizeof(StorageType) == 8);

template<typename StorageType>
requires EndianIntegral<StorageType>
StorageType ReverseBytes(StorageType const & value);

template<>
uint16_t ReverseBytes<uint16_t>(uint16_t const & value) { return __builtin_bswap16(value); }

template<>
int16_t ReverseBytes<int16_t>(int16_t const & value) { return __builtin_bswap16(value); }

template<>
uint32_t ReverseBytes<uint32_t>(uint32_t const & value) { return __builtin_bswap32(value); }

template<>
int32_t ReverseBytes<int32_t>(int32_t const & value) { return __builtin_bswap32(value); }

template<>
uint64_t ReverseBytes<uint64_t>(uint64_t const & value) { return __builtin_bswap64(value); }

template<>
int64_t ReverseBytes<int64_t>(int64_t const & value) { return __builtin_bswap64(value); }

template<typename StorageType>
requires EndianIntegral<StorageType>
class OtherEndian:
  //tao::operators::left_shiftable< OtherEndian<StorageType> >,
  //..tao::operators::left_shiftable< OtherEndian<StorageType>, int >,
  tao::operators::bitwise< OtherEndian<StorageType>, StorageType >
{
private:
  StorageType value = 0;

public:
  // Explicit Conversions

  StorageType getNativeValue() const { return ReverseBytes(value); }
  void setNativeValue(StorageType const native) { value = ReverseBytes(native); }

  StorageType getEncodedValue() const { return value; }
  void setEncodedValue(StorageType const encodedValue) { value = encodedValue; }

  OtherEndian() = default;

  // Alternative Constructors for Integrals and OtherEndian types

  OtherEndian(EndianIntegral auto const native) :
    value(ReverseBytes(static_cast<StorageType>(native)))
  {
  }

  OtherEndian & operator=(EndianIntegral auto const rhs) {
    value = OtherEndian(rhs).value;
    return *this;
  }

  template <typename AltStorageType>
  OtherEndian(OtherEndian<AltStorageType> const other) :
    OtherEndian(static_cast<AltStorageType>(other))
  {
  }

  template <typename AltStorageType>
  OtherEndian & operator=(OtherEndian<AltStorageType> const rhs) {
    value = OtherEndian(rhs).value;
    return *this;
  }

  // Direct initialization from non null pointer
  OtherEndian(uint8_t const * const data) {
    assert(data != nullptr);
    std::memcpy(&value, data, sizeof(StorageType));
  }

  // Implicit Conversion
  operator StorageType() const { return getNativeValue(); }

  // Unary Plus
  OtherEndian operator+() const { return *this; }

  // Unary Minus
  OtherEndian operator-() const { return -getNativeValue(); }

  // Prefix Increment
  OtherEndian& operator++() {
    StorageType temp = getNativeValue();
    temp++;
    setNativeValue(temp);
    return *this; // return new value by reference
  }

  // Postfix Increment
  OtherEndian operator++(int) {
    OtherEndian old = *this; // copy old value
    operator++();  // prefix increment
    return old;    // return old value
  }

  // Prefix Decrement
  OtherEndian& operator--() {
    StorageType temp = getNativeValue();
    temp--;
    setNativeValue(temp);
    return *this; // return new value by reference
  }

  // Compound Sum Assignment
  OtherEndian & operator+=(StorageType const & rhs) {
    setNativeValue(getNativeValue() + rhs);
    return *this;
  }

  // Compound Difference Assignment
  OtherEndian & operator-=(StorageType const & rhs) {
    setNativeValue(getNativeValue() - rhs);
    return *this;
  }

  // Compound Product Assignment
  OtherEndian & operator*=(StorageType const & rhs) {
    setNativeValue(getNativeValue() * rhs);
    return *this;
  }

  // Compound Quotient Assignment
  OtherEndian & operator/=(StorageType const & rhs) {
    setNativeValue(getNativeValue() / rhs);
    return *this;
  }

  OtherEndian operator--(int) {
    OtherEndian old = *this; // copy old value
    operator--();  // prefix decrement
    return old;    // return old value
  }

  // Bitwise NOT
  OtherEndian operator~() const {
    OtherEndian result;
    result.value = ~value;
    return result;
  }

  // Bitwise AND
  OtherEndian & operator&=(OtherEndian const & rhs) {
    value &= rhs.value;
    return *this;
  }

  OtherEndian & operator&=(StorageType const & rhs) {
    value &= ReverseBytes(rhs);
    return *this;
  }

  // Bitwise OR
  OtherEndian & operator|=(OtherEndian const & rhs) {
    value |= rhs.value;
    return *this;
  }

  OtherEndian & operator|=(StorageType const & rhs) {
    value |= ReverseBytes(rhs);
    return *this;
  }

  // Bitwise XOR
  OtherEndian & operator^=(OtherEndian const & rhs) {
    value ^= rhs.value;
    return *this;
  }

  OtherEndian & operator^=(StorageType const & rhs) {
    value ^= ReverseBytes(rhs);
    return *this;
  }

  // Left Bit-Shift

  //OtherEndian operator<<(std::integral auto const & shifts) const {
  //  if (std::cmp_less(shifts, 1)) return *this;
  //  if (std::cmp_greater_equal(shifts, sizeof(StorageType) * CHAR_BIT)) return 0;
  //  return getNativeValue() << shifts;
  //}

  //OtherEndian operator<<(OtherEndian const & shifts) const {
  //  return operator<<(static_cast<int const>(shifts));
  //}

  OtherEndian & operator<<=(int const & shifts) {
    int64_t fred = getNativeValue() << shifts;
    std::cout << "SWS fred = " << fred << std::endl;


    if (std::cmp_less(shifts, 1)) return *this;
    if (std::cmp_greater_equal(shifts, sizeof(StorageType) * CHAR_BIT)) return *this;
    

    setNativeValue(getNativeValue() << shifts);

    

    return *this;
  }

  OtherEndian & operator<<=(OtherEndian const & shifts) {
    return operator<<=(static_cast<int const>(shifts));
  }

  // Right Bit-Shift

  OtherEndian & operator>>=(int const & shifts) {
    if (std::cmp_less(shifts, 1)) return *this;
    if (std::cmp_greater_equal(shifts, sizeof(StorageType) * CHAR_BIT)) return *this;
    setNativeValue(getNativeValue() >> shifts);
    return *this;
  }

  OtherEndian & operator>>=(OtherEndian const & shifts) {
    return operator>>=(static_cast<int const>(shifts));
  }
};

template <typename IntegralType>
concept OtherEndianType = std::same_as<IntegralType, OtherEndian< int16_t>> ||
                          std::same_as<IntegralType, OtherEndian<uint16_t>> ||
                          std::same_as<IntegralType, OtherEndian< int32_t>> ||
                          std::same_as<IntegralType, OtherEndian<uint32_t>> ||
                          std::same_as<IntegralType, OtherEndian< int64_t>> ||
                          std::same_as<IntegralType, OtherEndian<uint64_t>>;

template <typename IntegralType>
auto GetNativeValue(IntegralType const & value)
{
  if constexpr (EndianIntegral<IntegralType>) {
    return value;
  }

  if constexpr(OtherEndianType<IntegralType>) {
    return value.getNativeValue();
  }

  // Build time errors

  static_assert(EndianIntegral<IntegralType> || OtherEndianType<IntegralType>,
                "\n\n\33[1;31mError: Supplied argument is not representable as an Endian Integral!\33[0m\n\n");
}

template <typename IntegralType>
auto GetEncodedValue(IntegralType const & value)
{
  if constexpr (EndianIntegral<IntegralType>) {
    return ReverseBytes(value);
  }

  if constexpr(OtherEndianType<IntegralType>) {
    return value.getEncodedValue();
  }

  // Build time errors

  static_assert((EndianIntegral<IntegralType> || OtherEndianType<IntegralType>),
                "\n\n\33[1;31mError: Supplied argument is not representable as an Endian Integral!\33[0m\n\n");
}

template <typename IntegralType>
auto GetBigEndianValue(IntegralType const & value)
{
  if constexpr (EndianIntegral<IntegralType>) {
    if constexpr (std::endian::native == std::endian::big) {
      return value;
    } else {
      return ReverseBytes(value);
    }
  }

  if constexpr(OtherEndianType<IntegralType>) {
    if constexpr (std::endian::native == std::endian::big) {
      return value.getNativeValue();
    } else {
      return value.getEncodedValue();
    }
  }

  // Build time errors

  static_assert(EndianIntegral<IntegralType> || OtherEndianType<IntegralType>,
                "\n\n\33[1;31mError: Supplied argument is not representable as a Big-Endian Integral!\33[0m\n\n");
}

template <typename IntegralType>
auto GetLittleEndianValue(IntegralType const & value)
{
  if constexpr (EndianIntegral<IntegralType>) {
    if constexpr (std::endian::native == std::endian::little) {
      return value;
    } else {
      return ReverseBytes(value);
    }
  }

  if constexpr(OtherEndianType<IntegralType>) {
    if constexpr (std::endian::native == std::endian::little) {
      return value.getNativeValue();
    } else {
      return value.getEncodedValue();
    }
  }

  // Build time errors

  static_assert(EndianIntegral<IntegralType> || OtherEndianType<IntegralType>,
                "\n\n\33[1;31mError: Supplied argument is not representable as a Little-Endian Integral!\33[0m\n\n");
}

}} // namespace culyun::endian

namespace {

// Typedef be, le, and oe equivalents to the fixed-width integral types

using uint16be_t = std::conditional_t<std::endian::native == std::endian::big, uint16_t, culyun::endian::OtherEndian<uint16_t>>;
using int16be_t = std::conditional_t<std::endian::native == std::endian::big, int16_t, culyun::endian::OtherEndian<int16_t>>;

using uint32be_t = std::conditional_t<std::endian::native == std::endian::big, uint32_t, culyun::endian::OtherEndian<uint32_t>>;
using int32be_t = std::conditional_t<std::endian::native == std::endian::big, int32_t, culyun::endian::OtherEndian<int32_t>>;

using uint64be_t = std::conditional_t<std::endian::native == std::endian::big, uint64_t, culyun::endian::OtherEndian<uint64_t>>;
using int64be_t = std::conditional_t<std::endian::native == std::endian::big, int64_t, culyun::endian::OtherEndian<int64_t>>;

using uint16le_t = std::conditional_t<std::endian::native == std::endian::little, uint16_t, culyun::endian::OtherEndian<uint16_t>>;
using int16le_t = std::conditional_t<std::endian::native == std::endian::little, int16_t, culyun::endian::OtherEndian<int16_t>>;

using uint32le_t = std::conditional_t<std::endian::native == std::endian::little, uint32_t, culyun::endian::OtherEndian<uint32_t>>;
using int32le_t = std::conditional_t<std::endian::native == std::endian::little, int32_t, culyun::endian::OtherEndian<int32_t>>;

using uint64le_t = std::conditional_t<std::endian::native == std::endian::little, uint64_t, culyun::endian::OtherEndian<uint64_t>>;
using int64le_t = std::conditional_t<std::endian::native == std::endian::little, int64_t, culyun::endian::OtherEndian<int64_t>>;

using uint16oe_t = culyun::endian::OtherEndian<uint16_t>;
using int16oe_t = culyun::endian::OtherEndian<int16_t>;

using uint32oe_t = culyun::endian::OtherEndian<uint32_t>;
using int32oe_t = culyun::endian::OtherEndian<int32_t>;

using uint64oe_t = culyun::endian::OtherEndian<uint64_t>;
using int64oe_t = culyun::endian::OtherEndian<int64_t>;

}
