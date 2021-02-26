#pragma once

#include <bit>
#include <climits>
#include <cstdint>
#include <type_traits>

namespace culyun { namespace endian {

template<typename IntegralType>
concept EndianIntegral =
    std::is_integral_v<IntegralType> && (sizeof(IntegralType) == 2 || sizeof(IntegralType) == 4 || sizeof(IntegralType) == 8);

template<typename IntegralType>
requires EndianIntegral<IntegralType>
IntegralType ReverseBytes(IntegralType const & value);

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

template<typename IntegralType>
requires EndianIntegral<IntegralType>
class OtherEndian
{
private:
  IntegralType oeValue = 0;

public:

  //////////////////////////////
  //
  // Default copy and move constructors and assignment operators

  OtherEndian(OtherEndian const & other) = default;
  OtherEndian & operator=(OtherEndian const & rhs) = default;

  OtherEndian(OtherEndian && other) = default;
  OtherEndian & operator=(OtherEndian && rhs) = default;

  //////////////////////////////
  //
  // Integral Constructors and Operator Overloads

  // Copy from IntegralType constructor

  OtherEndian(IntegralType const & native) : oeValue(ReverseBytes(native)) {}

  // Implicit IntegralType Conversion
  operator IntegralType() const { return ReverseBytes(oeValue); }

  // Unary Plus
  OtherEndian operator+() const { return +ReverseBytes(oeValue); }

  // Unary Minus
  OtherEndian operator-() const { return -ReverseBytes(oeValue); }

  // Prefix Increment
  OtherEndian& operator++() {
    IntegralType temp = ReverseBytes(oeValue);
    temp++;
    oeValue = ReverseBytes(temp);
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
    IntegralType temp = ReverseBytes(oeValue);
    temp--;
    oeValue = ReverseBytes(temp);
    return *this; // return new value by reference
  }

  // Postfix Decrement
  OtherEndian operator--(int) {
    OtherEndian old = *this; // copy old value
    operator--();  // prefix decrement
    return old;    // return old value
  }

  // Bitwise NOT
  OtherEndian operator~() const {
    return ~oeValue;
  }

  // Bitwise AND
  OtherEndian operator&(OtherEndian const & rhs) const {
    return oeValue & rhs.oeValue;
  }

  // Bitwise OR
  OtherEndian operator|(OtherEndian const & rhs) const {
    return oeValue | rhs.oeValue;
  }

  // Bitwise XOR
  OtherEndian operator^(OtherEndian const & rhs) const {
    return oeValue ^ rhs.oeValue;
  }

  // Left Bit-Shift
  OtherEndian operator<<(unsigned const & numShifts) const {
    if (numShifts >= static_cast<int>(sizeof(IntegralType) * CHAR_BIT)) return 0;

    return ReverseBytes(oeValue) << numShifts;
  }

  OtherEndian operator<<(int const & numShifts) const {
    if (numShifts < 1) return *this;

    return operator<<(static_cast<unsigned>(numShifts));
  }

  OtherEndian operator<<(OtherEndian const & numShifts) const {
    return operator<<(static_cast<int const>(numShifts));
  }

  // Right Bit-Shift
  OtherEndian operator>>(unsigned const & numShifts) const {
    if (numShifts >= static_cast<int>(sizeof(IntegralType) * CHAR_BIT)) return 0;

    return ReverseBytes(oeValue) >> numShifts;
  }

  OtherEndian operator>>(int const & numShifts) const {
    if (numShifts < 1) return *this;

    return operator>>(static_cast<unsigned>(numShifts));
  }

  OtherEndian operator>>(OtherEndian const & numShifts) const {
    return operator>>(static_cast<int const>(numShifts));
  }
};

}} // namespace culyun::endian

namespace {

// Typedef be and le equivalents to the fixed-width integral types

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

}
