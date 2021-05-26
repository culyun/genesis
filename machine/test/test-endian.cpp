#include <string>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <boost/ut.hpp>

#include <machine/endian.hpp>
#include <misc/type-names.hpp>

using namespace boost::ut::literals;
using namespace boost::ut::operators::terse;
using namespace boost::ut::bdd;
using namespace boost::ut;
using namespace culyun;

#include <utility>

namespace ut = boost::ut;

namespace cfg {
struct printer : ut::printer {
  template <class T>
  auto& operator<<(T&& t) {
    std::cerr << std::forward<T>(t);
    return *this;
  }
};
}  // namespace cfg

//template <>
//auto ut::cfg<ut::override> = ut::runner<ut::reporter<cfg::printer>>{};

namespace cfg {
class reporter : public ut::reporter<cfg::printer> {
private:
  using Base = ut::reporter<cfg::printer>;

public:
  auto on(ut::events::test_begin begin) -> void { Base::on(begin); }

  auto on(ut::events::test_run run) -> void {
    printer_ << "\n   \"" << run.type << " " << run.name << "\"...";
  }

  auto on(ut::events::test_skip skip) -> void { Base::on(skip); }

  auto on(ut::events::test_end end) -> void { Base::on(end); }

  template <class TMsg>
  auto on(ut::events::log<TMsg> log) -> void { Base::on(log); }

  template <class TExpr>
  auto on(ut::events::assertion_pass<TExpr> assertion_pass) -> void { Base::on(assertion_pass); }

  template <class TExpr>
  auto on(ut::events::assertion_fail<TExpr> assertion_fail) -> void { Base::on(assertion_fail); }

  auto on(ut::events::fatal_assertion fatal_assertion) -> void { Base::on(fatal_assertion); }

  auto on(ut::events::exception exception) -> void { Base::on(exception); }

  auto on(ut::events::summary summary) -> void { Base::on(summary); }
};
}  // namespace cfg

template <>
auto ut::cfg<ut::override> = ut::runner<cfg::reporter>{};




int main() {

  "ERD-ENDIAN-1000: Other-Endian integrals can be used to transport data"_test = []<class IntegralType> {
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
  //"args and types"_test = []<class TArg>(TArg arg) {

  "ERD-ENDIAN-1001: Other-Endian integrals use the reverse of native byte ordering"_test
    = [] <typename EndianIntegral>(EndianIntegral const testValue)
  {
    given("a " + type_support::friendly_name<EndianIntegral>() + " value = " + std::to_string(testValue)) = [&]
    {
      when("the value is assigned to an equivalent Other-Endian integral") = [&]
      {
        endian::OtherEndian<EndianIntegral> other = testValue;

        then("the byte-ordering should be reversed") = [&]
        {
          EndianIntegral const encoded = other.getEncodedValue();
          //expect(native == encoded);
          expect(encoded == endian::ReverseBytes(testValue));
          expect(endian::ReverseBytes(encoded) == testValue);
        };
      };
    };
  } | std::tuple{uint16_t(42), uint32_t(42), uint64_t(0xBAADF00DU), int16_t(-1), int32_t(-42), int64_t(0)};


  return 0;
}

