#include "log.h"

#include <ctime>
#include <unordered_map>
#include <fstream>

namespace logging {
  std::unordered_map<std::string, std::ofstream> s_logs;
}

void logging::restart(const std::string& filename) {
  std::ofstream& file = s_logs[filename]; 
  file.open(filename);

  time_t now = time(NULL);
  char* date = ctime(&now);

  file << "LOG FILE - local time: " << date << std::endl;
  file.close();
}

void logging::write(const std::string& filename, const std::string& text) {
  // If the file doesn't exist create it.
  if (s_logs.find(filename) == s_logs.end()) {
    logging::restart(filename);
  }

  std::ofstream& file = s_logs[filename];

  file.open(filename, std::ios_base::app);
  file << text << std::endl;
  file.close();
}
