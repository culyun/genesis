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

#include "free-bit-list.hpp"

namespace culyun::memory {

using namespace culyun;

template <typename StorageBackend>
class FreeBitListManager
{
public:

  int reserveSmallestFit(unsigned const size);

  int reserveLargestFit(unsigned const size);

  int reserveFirstFit(unsigned const size);

  int reserveLastFit(unsigned const size);

private:

  StorageBackend::StorageType read() const {
    return static_cast<StorageBackend *>(this)->doRead();
  }

  bool write(StorageBackend::StorageType const & newBitList) {
    return static_cast<StorageBackend *>(this)->doWrite(newBitList);
  }
};

