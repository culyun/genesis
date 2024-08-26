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

#include "util/archetype.hpp"
#include "atomic/helpers.hpp"
#include "bit/helpers.hpp"
#include "free-bit-list.hpp"

namespace culyun::memory {

class memory_resource
{
private:

  virtual void * do_allocate(std::size_t const bytes, std::size_t const alignment) = 0;

  virtual void do_deallocate(void * const p, std::size_t const bytes, std::size_t const alignment) = 0;

  virtual bool do_is_equal(memory_resource const & other) const noexcept = 0;

public:
  memory_resource() = default;

  memory_resource(memory_resource const &) = default;

  void * allocate(std::size_t const bytes, std::size_t const alignment = alignof(std::max_align_t)) {
    return do_allocate(bytes, alignment);
  }

  void deallocate(void * const p, std::size_t const bytes, std::size_t const alignment = alignof(std::max_align_t)) {
    return do_deallocate(p, bytes, alignment);
  }

  bool is_equal(memory_resource const & other) const noexcept {
    return do_is_equal(other);
  }
};


class SmallPool : public memory_resource, public util::Archetype< SmallPool >
{
private:
  using AtomicFreeList = ;

  std::byte * pool = nullptr;
  size_t poolSize = 0;
  bit::AtomicFreeBitList<uint32_t> freeBitList = {0}; // Each zero bit identifies a free region in the pool

  virtual void * do_allocate(std::size_t const bytes, std::size_t const alignment);

  virtual void do_deallocate(void * const p, std::size_t const bytes, std::size_t const alignment);

  virtual bool do_is_equal(memory_resource const & other) const noexcept {
    return &other == this;
  }

public:

  SmallPool(void * const buffer, size_t const buffer_size);

  SmallPool(SmallPool const &) = delete;
};

class Waypoint 
{
public:

  std::byte data[256];


  void * operator new (size_t size);
  void   operator delete (void * ptr);
  void * operator new [] (size_t size);
  void   operator delete [] (void * ptr);

};

} 
