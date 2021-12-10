#pragma once
// vim: set expandtab:tabstop=2:shiftwidth=2

#include <cassert>
#include <type_traits>

namespace culyun::util {

class NullSecondary {};

template <typename Primary, typename Secondary = NullSecondary>
class Archetype
{
private:

  static inline Primary * primary = nullptr;
  static inline Secondary * secondary = nullptr;
  static inline Archetype * owner = nullptr;

public:

  Archetype(Primary * const elected_primary, Secondary * const elected_secondary = nullptr)
  {
    primary = elected_primary;

    if constexpr (!std::is_same_v<Secondary, NullSecondary>) {
       secondary = elected_secondary;
    }

    owner = this;
  }

  Archetype() : Archetype(static_cast<Primary *>(this)) {}

  ~Archetype() {
    if (owner != this) return;
    primary = nullptr;
    secondary = nullptr;
  }

  // Archetype<Primary> is non-copyable and non-moveable
  Archetype(Archetype const &) = delete;
  Archetype & operator=(Archetype const &) = delete;
  Archetype(Archetype &&) = delete;
  Archetype & operator=(Archetype &&) = delete;

  static Primary * Get_Primary_Ptr() {
    return primary;
  }

  static Primary & Get_Primary() {
    if (primary == nullptr) {
      assert(!"util::Archetype - dereferencing null primary");
    }

    return *primary;
  }

  static Secondary * Get_Secondary_Ptr() {
    return secondary;
  }

  static Secondary & Get_Secondary() {
    if (secondary == nullptr) {
      assert(!"util::Archetype - dereferencing null secondary");
    }

    return *secondary;
  }

};

template <typename Primary, typename Secondary = NullSecondary>
Primary & Lookup()
{
  return Archetype<Primary, Secondary>::Get_Primary();
}

template <typename Primary, typename Secondary = NullSecondary>
Primary * Lookup_Ptr()
{
  return Archetype<Primary, Secondary>::Get_Primary_Ptr();
}

template <typename Primary, typename Secondary>
Primary & Lookup_Primary()
{
  return Archetype<Primary, Secondary>::Get_Primary();
}

template <typename Primary, typename Secondary>
Primary * Lookup_Primary_Ptr()
{
  return Archetype<Primary, Secondary>::Get_Primary_Ptr();
}

template <typename Primary, typename Secondary>
Secondary & Lookup_Secondary()
{
  return Archetype<Primary, Secondary>::Get_Secondary();
}

template <typename Primary, typename Secondary>
Secondary * Lookup_Secondary_Ptr()
{
  return Archetype<Primary, Secondary>::Get_Secondary_Ptr();
}

}
