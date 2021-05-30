#include <string>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/ut.hpp>

#include <machine/endian.hpp>
#include <misc/ansi-codes.hpp>
#include <misc/type-names.hpp>
#include <misc/ut-helpers.hpp>

using namespace boost::ut::literals;
using namespace boost::ut::operators::terse;
using namespace boost::ut::bdd;
using namespace boost::ut;
using namespace culyun;

void execute(auto && callable, auto && argsList)
{
  std::apply(
    /* fold argsList */ [&callable](auto && ... args) {
      (callable(args), ...);
    },
    std::move(argsList)
  );
}

void execute2(auto && callable, auto && ... args)
{
  (callable(args), ...);
}

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

  /////////////////////////////////////////////////////////////////////////////

  bdd::gherkin::steps steps = [](auto& steps) {
    steps.feature("Addition") = [&] {
      steps.scenario("*") = [&] {
        steps.given("int32oe_t value = {value}") = [&](int32_t value) {
          int32oe_t lhs = value;
          steps.when("I add {value}") = [&](int32_t value2) {
            int32_t sum = lhs + value2; // Regular addition via implicit conversion to int32_t
            lhs += value2; // Compound Sum Assignment
            steps.then("I expect {value}") = [&](int32_t expected_sum) {
              expect(sum == expected_sum);
              expect(lhs == expected_sum);
            };
          };
        };
      };
    };
  };

  "ERD-ENDIAN-0005: endian::OtherEndian integrals can perform integral arithmetic"_test
    = steps |
  R"(
    Feature: Addition

      Scenario: int32oe_t + int32_t
        Given int32oe_t value = 42;
          When I add 2
          Then I expect 44
      Scenario: int32oe_t + int32_t
        Given int32oe_t value = 42;
          When I add -2
          Then I expect 40
      Scenario: int32oe_t + int32_t
        Given int32oe_t value = -54321;
          When I add -1
          Then I expect -54322

  )";

  execute(
    /* test = */ [](auto && args) {
      auto augend = std::get<0>(args);
      auto addend = std::get<1>(args);
      auto sum = std::get<2>(args);
      expect(augend + addend == sum);
    },
    /* argsList = */ std::tuple{
      std::tuple{1, 1, 2},
      std::tuple{int32_t(1), int64_t(2), 3}}
  );

  execute2(
    /* test = */ [](auto && args) {
      auto const augend = std::get<0>(args);
      auto const addend = std::get<1>(args);
      auto const sum = augend + addend;

      endian::OtherEndian<decltype(augend)> augend_oe = augend;
      endian::OtherEndian<decltype(addend)> addend_oe = addend;

      expect(augend_oe + addend == sum);
      expect(augend + addend_oe == sum);
      expect(augend_oe + addend_oe == sum);
    },
    /* with these args = */
      std::tuple{1, 1},
      std::tuple{int32_t(1), int16_t(2)}
  );

  return 0;
}
