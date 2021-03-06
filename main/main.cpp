

#include <iostream>
#include <iomanip>
#include <string>
#include <type_traits>
#include <concepts>

#include "../machine/endian.hpp"

#include "../machine/fixed-point.hpp"
using namespace machine;

template <FixedPrecisionTraits traits>
requires FixedPrecisionTraitsValidator<traits>
void printFixedPrecision(FixedPrecision<traits> const & number, std::string const & name = "number")
{
  std::cout << name << ".Traits:" << std::endl
            << " .isSigned " << std::string(number.Traits.isSigned ? "true" : "false")
            << std::dec
            << ", .bits " << number.Traits.bits
            << ", .power " << number.Traits.power
            << ", .minBits " << number.Traits.minBits
            << ", .maxBits " << number.Traits.maxBits
            << std::endl;

  std::cout << name << ".data: " << number.data << std::endl;
}

#define printFP(number) printFixedPrecision(number, #number)

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

 //constexpr FixedPrecisionTraits traits = {.bits = 10 };
 constexpr FixedPrecisionTraits traits = {.isSigned = false, .bits = 10, .maxBits = 25 };
 //FixedPrecision<{.bits = 10, .minBits = 5, .maxBits = 21}> number;
 FixedPrecision<traits> number_a(3);
 FixedPrecision<traits> number_b(5);

 auto product = FixedPrecision<traits>(3) * FixedPrecision<traits>(5);
 auto quotient = FixedPrecision<traits>(10) / FixedPrecision<traits>(3);

  printFP(number_a);
  printFP(product);
  printFP(quotient);

  static_assert(std::is_same_v<IEEE_754::_2008::Binary<32>, float>, "mah!!");
    

  float val = FixedPrecision<FixedPrecisionTraits{.isSigned = true, .bits = 32, .power=-1 }>(-12345679);

  std::cout << "Float val = " << std::setprecision(15) <<  val << std::endl;

  return 0;
}
