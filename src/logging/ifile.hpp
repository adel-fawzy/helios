#pragma once

#include <string>

namespace helios::logging {

/**
 * @brief A file that could be written to.
 *
 * @details
 * - The file will append the given message as it is, it will not add a new
 *   line.
 * - All public functions are synchronous.
 * - All public functions are not thead-safe.
 */
class IFile {
public:
  /**
   * @brief Default constructor
   */
  IFile() = default;

  /**
   * @brief Virtual default destructor.
   */
  virtual ~IFile() = default;

  /**
   * @brief Delete copy semantics
   */
  IFile(const IFile &) = delete;
  IFile &operator=(const IFile &) = delete;

  /**
   * @brief Default move semantics
   */
  IFile(IFile &&) = default;
  IFile &operator=(IFile &&) = default;

  /**
   * @brief Appends the given 'msg' to the file without adding a new line.
   *
   * @tparam Message The message type to be appended.
   * @param msg The message to be appended.
   */
  template <typename Message> void append(Message &&msg) {
    appendImpl(std::forward<Message>(msg));
  }

protected:
  /**
   * @brief 'append' implementation.
   */
  virtual void appendImpl(std::string msg) = 0;
};

} // namespace helios::logging::file
