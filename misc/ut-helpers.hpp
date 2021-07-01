#pragma once

#include <string>
#include <iostream>
#include <boost/ut.hpp>
#include <misc/ansi-codes.hpp>

using namespace boost::ut::literals;
using namespace boost::ut::operators::terse;
using namespace boost::ut::bdd;
using namespace boost::ut;
namespace ut = boost::ut;

namespace ut_helper {


struct printer : ut::printer
{
  template <class T>
  auto& operator<<(T&& t) {
    std::cerr << std::forward<T>(t);
    return *this;
  }
};

class reporter : public ut::reporter<printer>
{
private:
  using Base = ut::reporter<printer>;

public:
  auto on(ut::events::test_begin begin) -> void {
    printer_ << ansi_code::bold_cyan;
    Base::on(begin);
    printer_ << ansi_code::reset;
  }

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

} // namespace ut_helper

template <>
auto ut::cfg<ut::override> = ut::runner<ut_helper::reporter>{};

namespace ut_helper {

void log(std::string_view const & message)
{
  ut::log << message;
}

}
