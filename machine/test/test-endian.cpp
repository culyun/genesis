#include <string>
#include <iostream>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/ut.hpp>

#include <machine/endian.hpp>
#include <misc/text.hpp>
#include <misc/ansi-codes.hpp>
#include <misc/type-names.hpp>
#include <misc/ut-helpers.hpp>

#undef TEST_MISC_OTHER_ENDIAN_BEHAVIOURS

using namespace ansi_code;
using namespace culyun;

namespace ut = boost::ut;
using namespace boost::ut::bdd;

void execute1(auto && callable, auto && argsList)
{
  std::apply(
    /* fold argsList */ [&callable](auto && ... args) {
      (callable(args), ...);
    },
    std::move(argsList)
  );
}

void execute(auto && callable, auto && ... args)
{
  (callable(args), ...);
}

#if defined(TEST_MISC_OTHER_ENDIAN_BEHAVIOURS)

namespace {

//using namespace boost::ut::literals;
//using namespace boost::ut::operators::terse;
//using namespace boost::ut;

// The boost::ut::operators seem to interfere with the OtherEndian operators
// More work required to determine what's going on

void testMisc()
{
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
              ut::expect(sum == expected_sum);
              ut::expect(lhs == expected_sum);
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
}

} // anonymous namespace

#endif

void testStorage()
{
  /////////////////////////////////////////////////////////////////////////////

  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0001: endian::ReverseBytes reverses the byte ordering of the underlying data\n", reset));

  execute(/* test = */ [](auto const testValue) {
      using EndianIntegral = decltype(testValue);

      given("a " + type_support::friendly_name<EndianIntegral>() + " value = " + std::to_string(testValue)) = [&]
      {
        when("calling endian::ReverseBytes(value)") = [&]
        {
          auto const reversedValue = endian::ReverseBytes(testValue);

          then("the resulting byte-ordering should be reversed") = [&]
          {
            for (unsigned i = 0 ; i < sizeof(EndianIntegral) ; ++i) {
              ut::expect(*(reinterpret_cast<uint8_t const *>(&testValue) + i) == *(reinterpret_cast<uint8_t const *>(&reversedValue) + sizeof(EndianIntegral) - 1 - i));
            }
          };
        };
      };
    },
    /* testValues = */ uint16_t(0x1234UL), uint32_t(0x12345678UL), uint64_t(0xBAADF00DU), int16_t(-1), int32_t(-42), int64_t(0)
  );

  ///////////////////////////////////////////////////////////////////////////////

  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0002: endian::OtherEndian integrals encode data in reverse of native byte ordering\n", reset));

  execute(/* test = */ [](auto const testValue) {
      using EndianIntegral = decltype(testValue);

      given("a " + type_support::friendly_name<EndianIntegral>() + " value = " + std::to_string(testValue)) = [&]
      {
        when("the value is assigned to an equivalent endian::OtherEndian integral") = [&]
        {
          endian::OtherEndian<EndianIntegral> other = testValue;

          then("the byte-ordering should be reversed") = [&]
          {
            EndianIntegral const encoded = other.getEncodedValue();
            ut::expect(encoded == endian::ReverseBytes(testValue));
            ut::expect(endian::ReverseBytes(encoded) == testValue);
          };
        };
      };
    },
    /* testValues = */ uint16_t(42), uint32_t(42), uint64_t(0xBAADF00DU), int16_t(-1), int32_t(-42), int64_t(0)
  );

  /////////////////////////////////////////////////////////////////////////////

  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0003: endian::OtherEndian integrals can be used to transport data between native integrals", reset));

  execute(/* test = */ [](auto const testValue) {
      using EndianIntegral = decltype(testValue);
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

            std::remove_const_t<EndianIntegral> receivedValue = transport;
            ut::expect(receivedValue == testValue);
            receivedValue = transport2;
            ut::expect(receivedValue == testValue);

            // compare after implicit conversion

            ut::expect(transport == testValue);
            ut::expect(transport2 == testValue);

          };
        };
      };
    },
    /* testValues = */ uint16_t(42), uint32_t(42), uint64_t(0xBAADF00DU), int16_t(-1), int32_t(-42), int64_t(0)
  );
}

void testAddition(auto && args)
{
  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0137: endian::OtherEndian integrals can perform addition operations", reset));

  execute1(/* test = */ [](auto && args) {
      auto augend = std::get<0>(args);
      auto addend = std::get<1>(args);
      auto sum = augend + addend;

      ut_helper::log(text::concatenate(
          ansi_code::green, type_support::friendly_name<decltype(augend)>() , "(" , std::to_string(augend) , ")" ,
          ansi_code::bold_red, " + " ,
          ansi_code::cyan , type_support::friendly_name<decltype(addend)>() , "(" , std::to_string(addend) , ")" ,
          ansi_code::bold_red, " = " ,
          ansi_code::yellow , type_support::friendly_name<decltype(sum)>() , "(" , std::to_string(sum) , ")\n" ,
          ansi_code::reset
      ));

      decltype(sum) oeSum;

      // Test all permutations of native and oe arguments

      oeSum = endian::OtherEndian<decltype(augend)>(augend) + addend;
      ut::expect(sum == oeSum);

      oeSum = augend + endian::OtherEndian<decltype(addend)>(addend);
      ut::expect(sum == oeSum);

      oeSum = endian::OtherEndian<decltype(augend)>(augend) + endian::OtherEndian<decltype(addend)>(addend);
      ut::expect(sum == oeSum);

      // Test commutability with all argument permutations...

      oeSum = endian::OtherEndian<decltype(addend)>(addend) + augend;
      ut::expect(sum == oeSum);

      oeSum = addend + endian::OtherEndian<decltype(augend)>(augend);
      ut::expect(sum == oeSum);

      oeSum = endian::OtherEndian<decltype(addend)>(addend) + endian::OtherEndian<decltype(augend)>(augend);
      ut::expect(sum == oeSum);
    },
    args
  );
}

void testSubtraction(auto && args)
{
  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0138: endian::OtherEndian integrals can perform subtraction operations", reset));

  execute1(/* test = */ [](auto && args) {
      auto minuend = std::get<0>(args);
      auto subtrahend = std::get<1>(args);
      auto difference = minuend - subtrahend;

      ut_helper::log(text::concatenate(
          ansi_code::green, type_support::friendly_name<decltype(minuend)>() , "(" , std::to_string(minuend) , ")" ,
          ansi_code::bold_red, " - " ,
          ansi_code::cyan , type_support::friendly_name<decltype(subtrahend)>() , "(" , std::to_string(subtrahend) , ")" ,
          ansi_code::bold_red, " = " ,
          ansi_code::yellow , type_support::friendly_name<decltype(difference)>() , "(" , std::to_string(difference) , ")\n" ,
          ansi_code::reset
      ));

      decltype(difference) oeDifference;

      // Test all permutations of native and oe arguments

      oeDifference = endian::OtherEndian<decltype(minuend)>(minuend) - subtrahend;
      ut::expect(difference == oeDifference);

      oeDifference = minuend - endian::OtherEndian<decltype(subtrahend)>(subtrahend);
      ut::expect(difference == oeDifference);

      oeDifference = endian::OtherEndian<decltype(minuend)>(minuend) - endian::OtherEndian<decltype(subtrahend)>(subtrahend);
      ut::expect(difference == oeDifference);
    },
    args
  );
}

void testMultiplication(auto && args)
{
  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0139: endian::OtherEndian integrals can perform multiplication operations", reset));

  execute1(/* test = */ [](auto && args) {
      auto multiplicand = std::get<0>(args);
      auto multiplier = std::get<1>(args);
      auto product = multiplicand * multiplier;

      ut_helper::log(text::concatenate(
          ansi_code::green, type_support::friendly_name<decltype(multiplicand)>() , "(" , std::to_string(multiplicand) , ")" ,
          ansi_code::bold_red, " * " ,
          ansi_code::cyan , type_support::friendly_name<decltype(multiplier)>() , "(" , std::to_string(multiplier) , ")" ,
          ansi_code::bold_red, " = " ,
          ansi_code::yellow , type_support::friendly_name<decltype(product)>() , "(" , std::to_string(product) , ")\n" ,
          ansi_code::reset
      ));

      decltype(product) oeProduct;

      // Test all permutations of native and oe arguments

      oeProduct = endian::OtherEndian<decltype(multiplicand)>(multiplicand) * multiplier;
      ut::expect(product == oeProduct);

      oeProduct = multiplicand * endian::OtherEndian<decltype(multiplier)>(multiplier);
      ut::expect(product == oeProduct);

      oeProduct = endian::OtherEndian<decltype(multiplicand)>(multiplicand)  * endian::OtherEndian<decltype(multiplier)>(multiplier);
      ut::expect(product == oeProduct);

      // Test commutability with all argument permutations...

      oeProduct = endian::OtherEndian<decltype(multiplier)>(multiplier) * multiplicand;
      ut::expect(product == oeProduct);

      oeProduct = multiplier * endian::OtherEndian<decltype(multiplicand)>(multiplicand);
      ut::expect(product == oeProduct);

      oeProduct = endian::OtherEndian<decltype(multiplier)>(multiplier)  * endian::OtherEndian<decltype(multiplicand)>(multiplicand);
      ut::expect(product == oeProduct);
    },
    args
  );
}

void testDivision(auto && args)
{
  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0140: endian::OtherEndian integrals can perform division operations", reset));

  execute1(/* test = */ [](auto && args) {
      auto dividend = std::get<0>(args);
      auto divisor = std::get<1>(args);
      auto quotient = dividend / divisor;

      ut_helper::log(text::concatenate(
          ansi_code::green, type_support::friendly_name<decltype(dividend)>() , "(" , std::to_string(dividend) , ")" ,
          ansi_code::bold_red, " / " ,
          ansi_code::cyan , type_support::friendly_name<decltype(divisor)>() , "(" , std::to_string(divisor) , ")" ,
          ansi_code::bold_red, " = " ,
          ansi_code::yellow , type_support::friendly_name<decltype(quotient)>() , "(" , std::to_string(quotient) , ")\n" ,
          ansi_code::reset
      ));

      decltype(quotient) oeQuotient;

      // Test all permutations of native and oe arguments

      oeQuotient = endian::OtherEndian<decltype(dividend)>(dividend) / divisor;
      ut::expect(quotient == oeQuotient);

      oeQuotient = dividend / endian::OtherEndian<decltype(divisor)>(divisor);
      ut::expect(quotient == oeQuotient);

      oeQuotient = endian::OtherEndian<decltype(dividend)>(dividend) / endian::OtherEndian<decltype(divisor)>(divisor);
      ut::expect(quotient == oeQuotient);
    },
    args
  );
}

void testModulo(auto && args)
{
  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0141: endian::OtherEndian integrals can perform modulo operations", reset));

  execute1(/* test = */ [](auto && args) {
      auto dividend = std::get<0>(args);
      auto modulus = std::get<1>(args);
      auto remainder = dividend % modulus;

      ut_helper::log(text::concatenate(
          ansi_code::green, type_support::friendly_name<decltype(dividend)>() , "(" , std::to_string(dividend) , ")" ,
          ansi_code::bold_red, " % " ,
          ansi_code::cyan , type_support::friendly_name<decltype(modulus)>() , "(" , std::to_string(modulus) , ")" ,
          ansi_code::bold_red, " = " ,
          ansi_code::yellow , type_support::friendly_name<decltype(remainder)>() , "(" , std::to_string(remainder) , ")\n" ,
          ansi_code::reset
      ));

      decltype(remainder) oeRemainder;

      // Test all permutations of native and oe arguments

      oeRemainder = endian::OtherEndian<decltype(dividend)>(dividend) % modulus;
      ut::expect(remainder == oeRemainder);

      oeRemainder = dividend % endian::OtherEndian<decltype(modulus)>(modulus);
      ut::expect(remainder == oeRemainder);

      oeRemainder = endian::OtherEndian<decltype(dividend)>(dividend) % endian::OtherEndian<decltype(modulus)>(modulus);
      ut::expect(remainder == oeRemainder);
    },
    args
  );
}

#if 1

void testBitwiseOr(auto && args)
{
  ut_helper::log(text::concatenate(
      bold_cyan, "ERD-ENDIAN-0142: endian::OtherEndian integrals can perform bitwise OR operations", reset));

  execute1(/* test = */ [](auto && args) {
      auto lhs = std::get<0>(args);
      auto rhs = std::get<1>(args);
      auto result = lhs | rhs;

      ut_helper::log(text::concatenate(
          ansi_code::green, type_support::friendly_name<decltype(lhs)>() , "(" , std::to_string(lhs) , ")" ,
          ansi_code::bold_red, " | " ,
          ansi_code::cyan , type_support::friendly_name<decltype(rhs)>() , "(" , std::to_string(rhs) , ")" ,
          ansi_code::bold_red, " = " ,
          ansi_code::yellow , type_support::friendly_name<decltype(result)>() , "(" , std::to_string(result) , ")\n" ,
          ansi_code::reset
      ));

      decltype(result) oeResult;

      // Test all permutations of native and oe arguments

      oeResult = endian::OtherEndian<decltype(lhs)>(lhs) | rhs;
      ut::expect(result == oeResult);

      oeResult = lhs | endian::OtherEndian<decltype(rhs)>(rhs);
      ut::expect(result == oeResult);

      oeResult = endian::OtherEndian<decltype(lhs)>(lhs) | endian::OtherEndian<decltype(rhs)>(rhs);
      ut::expect(result == oeResult);

      // Test commutability with all argument permutations...

      oeResult = endian::OtherEndian<decltype(rhs)>(rhs) | lhs;
      ut::expect(result == oeResult);

      oeResult = rhs | endian::OtherEndian<decltype(lhs)>(lhs);
      ut::expect(result == oeResult);

      oeResult = endian::OtherEndian<decltype(rhs)>(rhs) | endian::OtherEndian<decltype(lhs)>(lhs);
      ut::expect(result == oeResult);
    },
    args
  );
}

#endif

int main() {

  auto const smallValuePairs = std::tuple{
      std::tuple{0, std::numeric_limits<int>::min(), 0},
      std::tuple{0, -2},
      std::tuple{0, -1},
      std::tuple{0, 1},
      std::tuple{0, 2},
      std::tuple{0, std::numeric_limits<int>::max()},

      std::tuple{std::numeric_limits<int>::min(), 1},
      std::tuple{-2, 1},
      std::tuple{-1, 1},
      std::tuple{0, 1},
      std::tuple{1, 1},
      std::tuple{2, 1},
      std::tuple{std::numeric_limits<int>::max(), 1},

      std::tuple{1, std::numeric_limits<int>::min(), 1},
      std::tuple{1, -2},
      std::tuple{1, -1},
      std::tuple{1, 1},
      std::tuple{1, 2},
      std::tuple{1, std::numeric_limits<int>::max()},
  };

  // Divide by zero Warning.  Do not use with division tests
  auto const zeroValuePairs = std::tuple{
      std::tuple{std::numeric_limits<int>::min(), 0},
      std::tuple{-2, 0},
      std::tuple{-1, 0},
      std::tuple{0, 0},
      std::tuple{1, 0},
      std::tuple{2, 0},
      std::tuple{std::numeric_limits<int>::max(), 0},
      std::tuple{0, std::numeric_limits<int>::min(), 0},
      std::tuple{0, -2},
      std::tuple{0, -1},
      std::tuple{0, 0},
      std::tuple{0, 1},
      std::tuple{0, 2},
      std::tuple{0, std::numeric_limits<int>::max()},
  };

  auto const miscPairs = std::tuple{
      std::tuple{1, int16_t(1)},
      std::tuple{32, -42},
      std::tuple{int32_t(1), int64_t(2)},
      std::tuple{1, int16_t(2)}
  };

  auto const args = std::tuple_cat(smallValuePairs, zeroValuePairs, miscPairs);
  auto const divisionArgs = std::tuple_cat(smallValuePairs, miscPairs);
  auto const bitwiseArgs = std::tuple_cat(smallValuePairs, zeroValuePairs); // Types must be the same rank

  /////////////////////////////////////////////////////////////////////////////

  // Storage, Conversion, and Type Punning

  testStorage();

  // Arithmetic Operations

  testAddition(args);
  testSubtraction(args);
  testMultiplication(args);
  testDivision(divisionArgs);
  testModulo(divisionArgs);

  // Bitwise Operations

  testBitwiseOr(bitwiseArgs);

  // Misc

#if defined(TEST_MISC_OTHER_ENDIAN_BEHAVIOURS)
  testMisc();
#endif

  return 0;
}
