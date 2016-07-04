#pragma once

#include <string>

namespace logging {
  void restart(const std::string& filename);
  void write(const std::string& filename, const std::string& text);
}
