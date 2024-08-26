#pragma once

namespace culyun::build_time {

#define TerminateBuildIf(failingCriteria, errorMessage) \
{ \
  static_assert(!(failingCriteria), \
                "\n\n\33[1;31m" errorMessage"\33[0m\n\n"); \
}

#define TerminateBuildIfTypeOf(instance, comparisonOperator /* either == or != */, type, errorMessage) \
{ \
  static_assert(std::is_same_v<decltype(instance), type> comparisonOperator false, \
                "\n\n\33[1;31m" errorMessage"\33[0m\n\n"); \
}

template<int result = 0>
struct Success
{
};

template<int reason = 1>
struct Error
{
};

}
