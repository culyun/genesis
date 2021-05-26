// Modified from StackOverflow comment:
//
// https://stackoverflow.com/a/56600402

#pragma once

#include <string_view>
#include <cstdint>
#include <type_traits>
#include <concepts>

namespace type_support {

template <typename T> constexpr std::string_view type_name();

template <>
constexpr std::string_view type_name<void>()
{ return "void"; }

namespace detail {

using type_name_prober = void;

template <typename T>
constexpr std::string_view wrapped_type_name()
{
#ifdef __clang__
    return __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#else
#error "Unsupported compiler"
#endif
}

constexpr std::size_t wrapped_type_name_prefix_length() {
    return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>());
}

constexpr std::size_t wrapped_type_name_suffix_length() {
    return wrapped_type_name<type_name_prober>().length()
        - wrapped_type_name_prefix_length()
        - type_name<type_name_prober>().length();
}

} // namespace detail

template <typename T>
constexpr std::string_view name() {
    constexpr auto wrapped_name = detail::wrapped_type_name<T>();
    constexpr auto prefix_length = detail::wrapped_type_name_prefix_length();
    constexpr auto suffix_length = detail::wrapped_type_name_suffix_length();
    constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
    return wrapped_name.substr(prefix_length, type_name_length);
}

template <typename T>
constexpr std::string friendly_name() {

  if constexpr (std::is_same_v<T, uint8_t>) {
    return "uint8_t";
  }

  if constexpr (std::is_same_v<T, uint16_t>) {
    return "uint16_t";
  }

  if constexpr (std::is_same_v<T, uint32_t>) {
    return "uint32_t";
  }

  if constexpr (std::is_same_v<T, uint64_t>) {
    return "uint64_t";
  }

  if constexpr (std::is_same_v<T, int8_t>) {
    return "int8_t";
  }

  if constexpr (std::is_same_v<T, int16_t>) {
    return "int16_t";
  }

  if constexpr (std::is_same_v<T, int32_t>) {
    return "int32_t";
  }

  if constexpr (std::is_same_v<T, int64_t>) {
    return "int64_t";
  }

  return std::string(name<T>());
}

} // namespace type
