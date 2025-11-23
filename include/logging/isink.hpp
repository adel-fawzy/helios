#pragma once

#include <string>
#include <utility>

namespace helios::logging {

/**
 * @class logging::ISink
 *
 * @brief Logs a message to a sink.
 *
 * @details
 * - All public functions are thread-safe.
 * - All public functions are asynchronous.
 *
 * @note The caller is responsible for adding the new line character. The sink
 *       will not enter a new line to the message.
 */
class ISink {
public:
  /**
   * @brief Default constructor
   */
  ISink() = default;

  /**
   * @brief Default destructor
   */
  virtual ~ISink() = default;

  /**
   * @brief Delete copy semantics
   */
  ISink(const ISink &) = delete;
  ISink &operator=(const ISink &) = delete;

  /**
   * @brief Default move semantics
   */
  ISink(ISink &&) = default;
  ISink &operator=(ISink &&) = default;

  /**
   * @brief Writes a message to the sink
   *
   * @tparam Message The message line to write
   */
  template <typename Message> void write(Message &&msg) {
    writeImpl(std::forward<Message>(msg));
  }

protected:
  virtual void writeImpl(std::string msg) = 0;
}; // class ISink

} // namespace helios::logging
