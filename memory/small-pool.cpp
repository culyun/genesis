#include "small-pool.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

#undef TRACK_SMALL_POOL_ALLOCATIONS
#define TRACK_SMALL_POOL_ALLOCATIONS

#include <new>

#if defined(TRACK_SMALL_POOL_ALLOCATIONS)

static std::atomic<uint32_t> allocations = {0};

#endif

namespace culyun::memory {

void * SmallPool::do_allocate(std::size_t const bytes, std::size_t const alignment)
{
  ;
}

void SmallPool::do_deallocate(void * const p, std::size_t const bytes, std::size_t const alignment)
{
  ;
}

SmallPool::SmallPool(void * const buffer, size_t const buffer_size) :
  pool(buffer),
  poolSize(buffer_size),
  freeList(0)
{
  assert(buffer != nullptr);
  assert(buffer_size > alignof(std::max_align_t));
}

///////////////////////////////////////////////////////////////////////////////

void * Waypoint::operator new(size_t size)
{
  void * const result = ::operator new(size);

#if defined(TRACK_SMALL_POOL_ALLOCATIONS)
  uint32_t const snapshot = ++allocations;
  fmt::print("Waypoint::new sz {}, pt {}, allocations {}, heap-used {}\n", size, result, snapshot, snapshot * sizeof(Waypoint));
#endif

  return result;
}

void Waypoint::operator delete (void * ptr)
{
  ::operator delete(ptr);

#if defined(TRACK_SMALL_POOL_ALLOCATIONS)
  uint32_t const snapshot = --allocations;
  fmt::print("Waypoint::delete pt {}, allocations {}, heap-used {}\n", ptr, snapshot, snapshot * sizeof(Waypoint));
#endif
}

void * Waypoint::operator new [](size_t size)
{
  void * const result = ::operator new [](size);

#if defined(TRACK_SMALL_POOL_ALLOCATIONS)
  fmt::print("Waypoint::new[] sz {}, pt {}\n", size, result);
#endif

  return result;
}

void Waypoint::operator delete [](void * ptr)
{
  ::operator delete [](ptr);

#if defined(TRACK_SMALL_POOL_ALLOCATIONS)
  fmt::print("Waypoint::delete[] {}\n", ptr);
#endif
}


}
