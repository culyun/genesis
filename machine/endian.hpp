#pragma once

#include <bit>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <concepts>
#include <utility>

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
class OtherEndian
{
private:
  StorageType value = 0;

public:

  //////////////////////////////
  //
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

  // Implicit StorageType Conversion
  operator StorageType() const { return ReverseBytes(value); }

  // Unary Plus
  OtherEndian operator+() const { return *this; }

  // Unary Minus
  OtherEndian operator-() const { return -ReverseBytes(value); }

  // Prefix Increment
  OtherEndian& operator++() {
    StorageType temp = ReverseBytes(value);
    temp++;
    value = ReverseBytes(temp);
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
    StorageType temp = ReverseBytes(value);
    temp--;
    value = ReverseBytes(temp);
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
    OtherEndian result;
    result.value = ~value;
    return result;
  }

  // Bitwise AND
  OtherEndian operator&(OtherEndian const & rhs) const {
    return value & rhs.value;
  }

  // Bitwise OR
  OtherEndian operator|(OtherEndian const & rhs) const {
    return value | rhs.value;
  }

  // Bitwise XOR
  OtherEndian operator^(OtherEndian const & rhs) const {
    return value ^ rhs.value;
  }

  // Left Bit-Shift

  OtherEndian operator<<(std::integral auto const & shifts) const {
    if (std::cmp_less(shifts, 1)) return *this;
    if (std::cmp_greater_equal(shifts, sizeof(StorageType) * CHAR_BIT)) return 0;
    return ReverseBytes(value) << shifts;
  }

  OtherEndian operator<<(OtherEndian const & shifts) const {
    return operator<<(static_cast<int const>(shifts));
  }

  // Right Bit-Shift

  OtherEndian operator>>(std::integral auto const & shifts) const {
    if (std::cmp_less(shifts, 1)) return *this;
    if (std::cmp_greater_equal(shifts, sizeof(StorageType) * CHAR_BIT)) return 0;
    return ReverseBytes(value) >> shifts;
  }

  OtherEndian operator>>(OtherEndian const & shifts) const {
    return operator>>(static_cast<int const>(shifts));
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
