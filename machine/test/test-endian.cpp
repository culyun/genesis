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
  /////////////////////////////////////////////////////////////////////////////

  "ERD-ENDIAN-0001: endian::ReverseBytes reverses the byte ordering of the underlying data"_test
    = [] <typename EndianIntegral>(EndianIntegral const testValue)
  {
    given("a " + type_support::friendly_name<EndianIntegral>() + " value = " + std::to_string(testValue)) = [&]
    {
      when("calling endian::ReverseBytes(value)") = [&]
      {
        auto const reversedValue = endian::ReverseBytes(testValue);

        then("the resulting byte-ordering should be reversed") = [&]
        {
          for (unsigned i = 0 ; i < sizeof(EndianIntegral) ; ++i) {
            expect(*(reinterpret_cast<uint8_t const *>(&testValue) + i) == *(reinterpret_cast<uint8_t const *>(&reversedValue) + sizeof(EndianIntegral) - 1 - i));
          }
        };
      };
    };
  } | std::tuple{uint16_t(0x1234UL), uint32_t(0x12345678UL), uint64_t(0xBAADF00DU), int16_t(-1), int32_t(-42), int64_t(0)};

  /////////////////////////////////////////////////////////////////////////////

  "ERD-ENDIAN-0002: endian::OtherEndian integrals encode data in reverse of native byte ordering"_test
    = [] <typename EndianIntegral>(EndianIntegral const testValue)
  {
    given("a " + type_support::friendly_name<EndianIntegral>() + " value = " + std::to_string(testValue)) = [&]
    {
      when("the value is assigned to an equivalent endian::OtherEndian integral") = [&]
      {
        endian::OtherEndian<EndianIntegral> other = testValue;

        then("the byte-ordering should be reversed") = [&]
        {
          EndianIntegral const encoded = other.getEncodedValue();
          expect(encoded == endian::ReverseBytes(testValue));
          expect(endian::ReverseBytes(encoded) == testValue);
        };
      };
    };
  } | std::tuple{uint16_t(42), uint32_t(42), uint64_t(0xBAADF00DU), int16_t(-1), int32_t(-42), int64_t(0)};

  /////////////////////////////////////////////////////////////////////////////

  "ERD-ENDIAN-0003: endian::OtherEndian integrals can be used to transport data between native integrals"_test
    = [] <typename EndianIntegral>(EndianIntegral const testValue)
  {
    given("a " + type_support::friendly_name<EndianIntegral>() + " value = " + std::to_string(testValue)) = [&]
    {
      when("the value is assigned to an endian::OtherEndian, and then from this intermediate back to a second native integral") = [&]
      {
        endian::OtherEndian<EndianIntegral> const transport = testValue;

        // Define an alias for the appropriate signed / unsigned 64-bit integral to use as template argument to endian::OtherEndian
        using xint64_t = std::conditional_t<std::is_signed_v<EndianIntegral>, int64_t, uint64_t>;

        // Transport via a "wide" endian::OtherEndian should not affect the underlying data
        endian::OtherEndian<xint64_t> transport2 = testValue;

        then("this second value should equal the original value") = [&]
        {
          // compare after back assignment

          EndianIntegral receivedValue = transport;
          expect(receivedValue == testValue);
          receivedValue = transport2;
          expect(receivedValue == testValue);

          // compare after implicit conversion

          expect(transport == testValue);
          expect(transport2 == testValue);

        };
      };
    };
  } | std::tuple{uint16_t(42), uint32_t(42), uint64_t(0xBAADF00DU), int16_t(-1), int32_t(-42), int64_t(0)};


  return 0;
}

