#include "file.hpp"

namespace helios::logging {

File::File(std::string path) : ofs_{path} {}

File::~File() {
  if (ofs_.is_open()) {
    ofs_.close();
  }
}

void File::appendImpl(std::string msg) {
  if (ofs_.is_open()) {
    ofs_ << msg;
    ofs_.flush();
  }
}

} // namespace helios::logging
