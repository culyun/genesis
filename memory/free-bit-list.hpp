#pragma once

#include <bit>
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <concepts>
#include <utility>
#include <cstddef>
#include <atomic>

#include "bit/helpers.hpp"
#include "atomic/helpers.hpp"

namespace culyun::memory {

using namespace culyun;

template <typename Bitmap = bit::AtomicAccessor<atomic::uintmax_lockfree_t>>
class AllocationMap
{
public:
  Bitmap start;
  Bitmap extent;
};

template <typename StorageBackend>
class FreeBitListManager
{
public:

  unsigned contentionLimit = 5;

  // reserve searches for [n] bits of space in the underlying Free Bit List.
  // returns:
  //  (a) the idx of the reserved space if successful, or
  //  (d) -1 if the supplied bits is outside the bounds of the underlying list
  //  (b) -2 if the underlying list is out of space, or
  //  (c) -3 if the contention limit was exceeded while attempting to update the list

  int reserve(unsigned const bits) {
    int result;

    if (bits == 0) {
      result = 0 ; return result; // Silly case
    }

    if (bits > (CHAR_BIT * sizeof(StorageBackend::IntegralType)) {
      result = -1; return result;
    }

    for (unsigned i = 0 ; i < contentionLimit ; ++i) {
      StorageBackend::IntegralType currentFreeList = readFreeList();
      auto freeBitIdx = bit::FindFreeBits(currentFreeList, bits);

      if (freeBitIdx < 0) {
        // Failure: Out of space
        result = -2; return result;
      }

      currentFreeList |= StorageBackend::IntegralType((1 << bits) - 1) << freeBitIdx;
      bool const writeSucceeded = setFreeListBits(currentFreeList);

      if (writeSucceeded) {
        // Success: Space reserved for [n] bits at freeBitIdx
        result = freeBitIdx; return result;
      }
    }

    // Failure : Too many reservation attempts
    result = -3; return result;
  }

  // release releases [n] bits of space at bitIdx in the underlying Free Bit List.
  // returns:
  //  (a) bitIdx if successful, or
  //  (d) -1 if the supplied freeBitIdx or bits is outside the bounds of the underlying list
  //  (b) -2 if any of the bits to release were already released, or
  //  (c) -3 if the contention limit was exceeded while attempting to update the list

  int release(unsigned const freeBitIdx, unsigned const bits) {
    int result;

    if (bits == 0) {
      result = 0 ; return result; // Silly case
    }

    if ((freeBitIdx + bits) > (CHAR_BIT * sizeof(StorageBackend::IntegralType)) {
      result = -1; return result;
    }

    StorageBackend::IntegralType const releaseBitMask = StorageBackend::IntegralType((1 << bits) - 1) << freeBitIdx;

    for (unsigned i = 0 ; i < contentionLimit ; ++i) {
      StorageBackend::IntegralType currentFreeList = readFreeList();

      if (currentFreeList & releaseBitMask != releaseBitMask) {
        result = -2; return result;
      }

      currentFreeList &= ~releaseBitMask;

      bool const writeSucceeded = writeFreeList(currentFreeList);

      if (writeSucceeded) {
        // Success: Space released for [n] bits at freeBitIdx
        result = freeBitIdx; return result;
      }
    }

    // Failure : Too many reservation attempts
    result = -3; return result;
  }

private:

  StorageBackend::IntegralType readFreeList() const {
    return static_cast<StorageBackend *>(this)->doRead();
  }

  bool writeFreeList(StorageBackend::IntegralType const & freeBitList) {
    return static_cast<StorageBackend *>(this)->doWrite(freeBitList);
  }
};



}
