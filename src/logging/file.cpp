#include "file.hpp"

namespace helios::logging {

File::File(std::string path) : path_{path} {
  ofs_.open(path_, std::ios::out | std::ios::app);
}

File::~File() {
  if (ofs_.is_open()) {
    ofs_.close();
  }
}

void File::appendImpl(std::string msg) {
  if (!ofs_.is_open()) {
    // Try to reopen (best-effort)
    ofs_.open(path_, std::ios::out | std::ios::app);
  }
  if (ofs_.is_open()) {
    ofs_ << msg;
    ofs_.flush();
  }
}

} // namespace helios::logging
