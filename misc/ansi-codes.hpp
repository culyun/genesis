#pragma once

#include <string_view>

namespace ansi_code {
  std::string_view reset                        = "\033[0m";

  std::string_view black                        = "\033[0;30m";
  std::string_view red                          = "\033[0;31m";
  std::string_view green                        = "\033[0;32m";
  std::string_view yellow                       = "\033[0;33m";
  std::string_view blue                         = "\033[0;34m";
  std::string_view magenta                      = "\033[0;35m";
  std::string_view cyan                         = "\033[0;36m";
  std::string_view white                        = "\033[0;37m";

  std::string_view bold_black                   = "\033[1;30m";
  std::string_view bold_red                     = "\033[1;31m";
  std::string_view bold_green                   = "\033[1;32m";
  std::string_view bold_yellow                  = "\033[1;33m";
  std::string_view bold_blue                    = "\033[1;34m";
  std::string_view bold_magenta                 = "\033[1;35m";
  std::string_view bold_cyan                    = "\033[1;36m";
  std::string_view bold_white                   = "\033[1;37m";

  std::string_view underline_black              = "\033[4;30m";
  std::string_view underline_red                = "\033[4;31m";
  std::string_view underline_green              = "\033[4;32m";
  std::string_view underline_yellow             = "\033[4;33m";
  std::string_view underline_blue               = "\033[4;34m";
  std::string_view underline_magenta            = "\033[4;35m";
  std::string_view underline_cyan               = "\033[4;36m";
  std::string_view underline_white              = "\033[4;37m";

  std::string_view background_black             = "\033[0;40m";
  std::string_view background_red               = "\033[0;41m";
  std::string_view background_green             = "\033[0;42m";
  std::string_view background_yellow            = "\033[0;43m";
  std::string_view background_blue              = "\033[0;44m";
  std::string_view background_magenta           = "\033[0;45m";
  std::string_view background_cyan              = "\033[0;46m";
  std::string_view background_white             = "\033[0;47m";

  std::string_view bright_black                 = "\033[0;90m";
  std::string_view bright_red                   = "\033[0;91m";
  std::string_view bright_green                 = "\033[0;92m";
  std::string_view bright_yellow                = "\033[0;93m";
  std::string_view bright_blue                  = "\033[0;94m";
  std::string_view bright_magenta               = "\033[0;95m";
  std::string_view bright_cyan                  = "\033[0;96m";
  std::string_view bright_white                 = "\033[0;97m";

  std::string_view bold_bright_black            = "\033[1;90m";
  std::string_view bold_bright_red              = "\033[1;91m";
  std::string_view bold_bright_green            = "\033[1;92m";
  std::string_view bold_bright_yellow           = "\033[1;93m";
  std::string_view bold_bright_blue             = "\033[1;94m";
  std::string_view bold_bright_magenta          = "\033[1;95m";
  std::string_view bold_bright_cyan             = "\033[1;96m";
  std::string_view bold_bright_white            = "\033[1;97m";

  std::string_view background_bright_black      = "\033[1;100m";
  std::string_view background_bright_red        = "\033[1;101m";
  std::string_view background_bright_green      = "\033[1;102m";
  std::string_view background_bright_yellow     = "\033[1;103m";
  std::string_view background_bright_blue       = "\033[1;104m";
  std::string_view background_bright_magenta    = "\033[1;105m";
  std::string_view background_bright_cyan       = "\033[1;106m";
  std::string_view background_bright_white      = "\033[1;107m";
}
