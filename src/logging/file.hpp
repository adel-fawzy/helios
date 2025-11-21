#pragma once

#include "ifile.hpp"
#include <fstream>

namespace helios::logging {

/**
 * @brief Appends a given message to a given file path.
 */
class File : public IFile {
public:
  /**
   * @brief Constructor.
   */
  File(std::string path);

  /**
   * @brief Constructor.
   */
  ~File() override;

protected:
  void appendImpl(std::string msg) override;

private:
  /**
   * @brief Path of the file to be appended.
   */
  const std::string path_;

  /**
   * @brief File stream.
   */
  std::ofstream ofs_;
};

} // namespace helios::logging
