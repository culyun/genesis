#pragma once

#include <cstdint>
#include <climits>
#include <type_traits>

#include "build_time/helpers.hpp"

namespace culyun::bit {

using namespace culyun;

template<unsigned bits>
constexpr auto IntegralLeast()
{
  auto result = [&]() {
    if constexpr (bits <= 8) {
        return uint_least8_t(CHAR_BIT * sizeof(uint_least8_t));
    } else if constexpr (bits <= 16) {
        return uint_least16_t(CHAR_BIT * sizeof(uint_least16_t));
    } else if constexpr (bits <= 32) {
        return uint_least32_t(CHAR_BIT * sizeof(uint_least32_t));
    } else if constexpr (bits <= 64) {
        return uint_least64_t(CHAR_BIT * sizeof(uint_least64_t));
    } else if constexpr (bits <= (CHAR_BIT * sizeof(uintmax_t))) {
        return uintmax_t(CHAR_BIT * sizeof(uintmax_t));
    } else {
        return build_time::Error<bits>{};
    }
  }();

  TerminateBuildIfTypeOf(result , == , build_time::Error<bits>,
                         "Error: No integral types of at least <bits> available on this platform!");

  return result;
}

template<typename IntegralType>
requires std::integral<IntegralType>
unsigned CountLeadingZeroes(IntegralType const value)
{
  if constexpr(std::is_signed_v<IntegralType>) {
    return CountLeadingZeroes(std::make_unsigned_t<IntegralType>(value));
  }

  if (value == 0) return CHAR_BIT * sizeof(IntegralType);

  if constexpr(sizeof(IntegralType) <= sizeof(unsigned)) {
    return __builtin_clz(value) - (CHAR_BIT * (sizeof(unsigned) - sizeof(IntegralType)));
  }

  if constexpr(sizeof(IntegralType) > sizeof(unsigned) && sizeof(IntegralType) <= sizeof(unsigned long)) {
    return __builtin_clzl(value) - (CHAR_BIT * (sizeof(unsigned long) - sizeof(IntegralType)));
  }

  if constexpr(sizeof(IntegralType) > sizeof(unsigned long) && sizeof(IntegralType) <= sizeof(unsigned long long)) {
    return __builtin_clzll(value) - (CHAR_BIT * (sizeof(unsigned long long) - sizeof(IntegralType)));
  }

  TerminateBuildIf(sizeof(IntegralType) > sizeof(unsigned long long),
                  "Error: No clz builtin for supplied arguement!");
}

template<typename IntegralType>
requires std::integral<IntegralType>
unsigned CountLeadingOnes(IntegralType const value)
{
  return CountLeadingZeroes(IntegralType(~value));
}

template<typename IntegralType>
requires std::is_unsigned_v<IntegralType>
int FindFreeBits(IntegralType freeBitList, unsigned const bits)
{
  int result;
  constexpr unsigned const MAX_FREE_BITS = CHAR_BIT * sizeof(IntegralType);
  unsigned freeBitListIdx = 0; // Start search

  // Paranoia ==> avoid any possibility of infinite loop by bounding iterations

  for (unsigned i = 0 ; i < MAX_FREE_BITS ; ++i) {
//    std::cout << "freeBitList " << hex_out_s(freeBitList) << std::endl;

    unsigned const remainingBits = MAX_FREE_BITS - freeBitListIdx;

    if (remainingBits < bits) {
      // Failure: No free space
      result = -1 ; return result;
    }

    unsigned const freeBits = CountLeadingZeroes(freeBitList);

    if (freeBits >= bits) {
      // Success: [n] bits can be stored in freeBitList at freeBitListIdx
      result = freeBitListIdx; return result;
    }

    if (freeBits > 0) {
      // There's a free space, but it's not big enough
      freeBitList = freeBitList << freeBits;
      freeBitListIdx += freeBits;
    }

    unsigned const usedBits = CountLeadingOnes(freeBitList);

//    std::cout << "freeBits " << freeBits << std::endl;
//    std::cout << "usedBits " << usedBits << std::endl;

    assert(usedBits > 0);
    assert(usedBits < remainingBits);

    freeBitList = freeBitList << usedBits;
    freeBitListIdx += usedBits;
  }

  // Failure: Shouldn't be able to get here
  assert(false);
  result = -2; return result;
}

int findFreeBits(uint32_t freeBitList, unsigned const bits)
{
  int result;
  unsigned freeBitListIdx = 0;

  for (unsigned i = 0 ; i < 32 ; ++i ) {
    unsigned const remainingBits = 32 - freeBitListIdx;

    if (remainingBits < bits) {
      result = -1 ; return result;
    }

    static_assert(sizeof(unsigned) == sizeof(uint32_t));

    unsigned const freeBits = freeBitList == 0 ? 0 : __builtin_clz(freeBitList);

    if (freeBits >= bits) {
      result = freeBitListIdx; return result;
    }

    if (freeBits > 0) {
      freeBitList = freeBitList << freeBits;
      freeBitListIdx += freeBits;
    }

    unsigned const usedBits = freeBitList == 0xFFFFFFFF ? 32 : __builtin_clz(uint32_t(~freeBitList));

    freeBitList = freeBitList << usedBits;
    freeBitListIdx += usedBits;
  }

  // Shouldn't get here
  assert(false);
  result = -2; return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename IntegralType>
struct Delta
{
  IntegralType current;
  IntegralType next;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename StorageType>
class Accessor
{
private:
  template <typename IntegralType> 
  struct AbstractInterface
  {
    using ValueType = IntegralType;

    virtual ValueType get() const = 0;

    virtual void set(ValueType const & nextValue) = 0;

    virtual bool trySet(Delta<ValueType> & value) = 0;

    virtual bool trySet(Delta<ValueType> && value) = 0;
  };

public:
  using Interface = AbstractInterface< std::conditional_t< std::is_signed_v< StorageType >,
          std::make_signed_t<atomic::uintmax_lockfree_t>,
          atomic::uintmax_lockfree_t>>;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename StorageType>
class TrivialAccessor : public Accessor<StorageType>::Interface
{
private:
  StorageType item = {0};

public:
  using ValueType = typename Accessor<StorageType>::Interface::ValueType;

  TrivialAccessor(StorageType const & value) : item(value) {}

  virtual ValueType get() const {
    return item;
  }

  virtual void set(ValueType const & nextValue) {
    item = nextValue;
  }

  virtual bool trySet(Delta<ValueType> & value) {
    if (item != value.current) return false;
    item = value.next;
    return true;
  }

  virtual bool trySet(Delta<ValueType> && value) {
      return trySet(value);
  }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename StorageType>
class AtomicAccessor : public Accessor<StorageType>::Interface
{
private:
  std::atomic<StorageType> item = {0};

public:
  using ValueType = typename Accessor<StorageType>::Interface::ValueType;

  AtomicAccessor(StorageType const & value) : item(value) {}

  virtual ValueType get() const {
    return item.load();
  }

  virtual void set(ValueType const & nextValue) {

    // currentValue is initialized to keep GCC 11.2 happy.
    // GCC whinges that currentValue may be uninitialized but only when StorageType is signed
    // Clang is ok with currentValue being uninitialized

    StorageType currentValue = 0;
    while (!item.compare_exchange_weak(currentValue, nextValue)) {}
  }

  virtual bool trySet(Delta<ValueType> & value) {

    // This if constexpr is an attempt to optimize for the case of StorageType and ValueType being the same
    // I suspect this is unecessary since currentValue is unused
    // However, it does change the interface slightly since value.current may be modified in the process.
    // Needs some more thought.

    if constexpr(std::is_same_v<StorageType, ValueType>) {
      return item.compare_exchange_strong(value.current, value.next);
    } else {
      StorageType currentValue = value.current;
      return item.compare_exchange_strong(currentValue, value.next);
    }
  }

  virtual bool trySet(Delta<ValueType> && value) {
      return trySet(value);
  }
};

}

