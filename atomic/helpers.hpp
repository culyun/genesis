#pragma once

#include <cstdint>
#include <climits>
#include <functional>
#include <type_traits>
#include <atomic>

#include "build_time/helpers.hpp"

namespace culyun::atomic {

using namespace culyun;

constexpr auto WidestLockFreeIntegral()
{
  auto result = [&]() {
    if constexpr (std::atomic<uintmax_t>::is_always_lock_free) {
        return uintmax_t(CHAR_BIT * sizeof(uintmax_t));
    } else if constexpr (std::atomic<uint_least64_t>::is_always_lock_free) {
        return uint_least64_t(CHAR_BIT * sizeof(uint_least64_t));
    } else if constexpr (std::atomic<uint_least32_t>::is_always_lock_free) {
        return uint_least32_t(CHAR_BIT * sizeof(uint_least32_t));
    } else if constexpr (std::atomic<uint_least16_t>::is_always_lock_free) {
        return uint_least16_t(CHAR_BIT * sizeof(uint_least16_t));
    } else if constexpr (std::atomic<uint_least8_t>::is_always_lock_free) {
        return uint_least8_t(CHAR_BIT * sizeof(uint_least8_t));
    } else {
        return build_time::Error<>{};
    }
  }();

  TerminateBuildIfTypeOf(result , == , build_time::Error<>,
                         "Error: No lock-free atomic integral types available on this platform!");

  return result;
}

static auto uintmax_lockfree_bits = WidestLockFreeIntegral();
using uintmax_lockfree_t = decltype(WidestLockFreeIntegral());

template<unsigned bits>
constexpr auto LockFreeIntegralLeast()
{
  auto result = [&]() {
    if constexpr (bits <= 8 && std::atomic<uint_least8_t>::is_always_lock_free) {
        return uint_least8_t(CHAR_BIT * sizeof(uint_least8_t));
    } else if constexpr (bits <= 16 && std::atomic<uint_least16_t>::is_always_lock_free) {
        return uint_least16_t(CHAR_BIT * sizeof(uint_least16_t));
    } else if constexpr (bits <= 32 && std::atomic<uint_least32_t>::is_always_lock_free) {
        return uint_least32_t(CHAR_BIT * sizeof(uint_least32_t));
    } else if constexpr (bits <= 64 && std::atomic<uint_least64_t>::is_always_lock_free) {
        return uint_least64_t(CHAR_BIT * sizeof(uint_least64_t));
    } else if constexpr (bits <= (CHAR_BIT * sizeof(uintmax_t)) && std::atomic<uintmax_t>::is_always_lock_free) {
        return uintmax_t(CHAR_BIT * sizeof(uintmax_t));
    } else {
        return build_time::Error<bits>{};
    }
  }();

  TerminateBuildIfTypeOf(result , == , build_time::Error<bits>,
                         "Error: No lock-free atomic integral types of at least <bits> available on this platform!");

  return result;
}

}
