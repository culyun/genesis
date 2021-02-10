

#include <iostream>
#include <string>
#include <type_traits>
#include <concepts>

#include "../machine/endian.hpp"

#include "../machine/fixed-point.hpp"
using namespace machine;

int main() 
{
  std::cout << std::string(std::is_same_v<int32_t, decltype(FastestIntegralType<SIGNED, 32>())> ? "int32_t" : "not int32_t") << std::endl;
  std::cout << std::string(std::is_same_v<int64_t, decltype(FastestIntegralType<SIGNED, 32>())> ? "int64_t" : "not int64_t") << std::endl;

  machine::FixedPoint<SIGNED, 28, 3> steve(9);
  FixedPoint<machine::SIGNED, 13, 5> nenny(5);

  auto george = steve * nenny;

  std::cout << "george.bits = " << george.Bits << ", .power " << george.Power << ", .value " << george.getData() << std::endl;


    uint32le_t fred = 1;
  int32le_t jo = 3;
  uint32be_t sum = fred << (jo);

  std::cout << "sum is " << sum << std::endl;

  return 0;
}
