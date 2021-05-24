#include <iostream>
#include <tuple>
#include <type_traits>
#include <boost/ut.hpp>
#include <machine/endian.hpp>

using namespace boost::ut::literals;
using namespace boost::ut::operators::terse;
using namespace boost::ut::bdd;
using namespace boost::ut;
using namespace culyun;

int main() {

  "ERD-ENDIAN-1000: Other-Endian Integrals can be used to transport data"_test = []<class IntegralType> {
    given("I have a nominally 16-bit unsigned number") = [] {
      when("I use an other-endian for transport") = [] {
        uint16_t source = 0x1234U;
        IntegralType intermediate = source;
        uint32_t destination = intermediate;
        then("the data is preserved") = [&] {
          expect(source == destination);
        };
      };
    };


    if constexpr(std::is_signed_v<IntegralType>) {
      given("I have a nominally 16-bit signed number") = [] {
        when("I use a signed other-endian for transport") = [] {
          int16_t source = -1;
          IntegralType intermediate = source;
          int32_t destination = intermediate;
          then("the data is preserved") = [&] {
            expect(source == destination);
          };
        };
      };
    }

  } | std::tuple<uint16oe_t, uint32oe_t, uint64oe_t, int16oe_t, int32oe_t, int64oe_t>{};

  return 0;
}

