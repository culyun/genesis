

#include <iostream>
#include <string>
#include <type_traits>
#include <concepts>

#include "../machine/endian.hpp"

#include "../machine/fixed-point.hpp"
using namespace machine;

int main() 
{
  if (std::is_same_v<int32_t, decltype(SmallestIntegralType<SIGNED, 31>())>) {
    std::cout << "int32_t is the smallest signed integral type supporting 31 bits of precision" << std::endl;
  } else if (std::is_same_v<int64_t, decltype(SmallestIntegralType<SIGNED, 31>())>) {
    std::cout << "int64_t is the smallest signed integral type supporting 31 bits of precision" << std::endl;
  }

  if (std::is_same_v<int32_t, decltype(FastestIntegralType<SIGNED, 31>())>) {
    std::cout << "int32_t is the fastest signed integral type supporting 31 bits of precision" << std::endl;
  } else if (std::is_same_v<int64_t, decltype(FastestIntegralType<SIGNED, 31>())>) {
    std::cout << "int64_t is the fastest signed integral type supporting 31 bits of precision" << std::endl;
  }

  machine::FixedPoint<SIGNED, 10, 6> steve(9);
  FixedPoint<machine::SIGNED, 10, 6> nenny(5);

  auto george = steve * nenny;

  decltype(george)::QNumberType herbert = george;

  std::cout << "george.bits = " << george.Bits << ", .power " << george.Power << ", .QNumber " << george.getQNumber() << std::endl;
  std::cout << "herbert " << herbert << ", 0x" << std::hex << herbert << std::endl;

  if (std::is_same_v<int32_t, decltype(george.getQNumber())>) {
    std::cout << "george storage is int32_t" << std::endl;
  } else if (std::is_same_v<int64_t, decltype(george.getQNumber())>) {
    std::cout << "george storage is int64_t" << std::endl;
  }

    uint32le_t fred = 1;
  int32le_t jo = 3;
  uint32be_t sum = fred << (jo);

  std::cout << "sum is " << sum << std::endl;

  return 0;
}
