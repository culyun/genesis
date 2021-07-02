#pragma once

#include <string>

namespace text {

std::string concatenate(auto &&... args)
{
  std::string result;
  result.reserve((std::string_view(args).size() + ...));
  (result.append(args), ...);
  return result;
}

}
