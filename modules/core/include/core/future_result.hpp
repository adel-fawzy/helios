#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <optional>

namespace helios::core {

#define THEN_POST(block)                                                       \
  ->then([this](auto &&__value) {                                              \
    this->post(                                                                \
        [result = std::forward<decltype(__value)>(__value), this] { block });  \
  })

#define THEN_POST_CALLABLE(fn)                                                 \
  ->then([this, fn = fn](auto &&__value) {                                     \
    this->post([result = std::forward<decltype(__value)>(__value),             \
                fn]() mutable { fn(result); });                                \
  })

/**
 * @class core::FutureResult
 *
 * @brief Result of an asynchronous operation.
 *
 * @details
 * - Created to be shared between the producer and the consumer threads.
 *
 * @note
 * - All public functions are synchronous.
 * - All public functions are thread-safe.
 */
template <typename ResultType> class FutureResult final {
public:
  /**
   * @brief Type alias for shared pointer to FutureResult<T>
   */
  using Ptr = std::shared_ptr<FutureResult<ResultType>>;

  /**
   * @brief Default constructor.
   */
  FutureResult();

  /**
   * @brief Default destructor.
   */
  ~FutureResult();

  /**
   * @brief Sets the result.
   *
   * @tparam value Value of the result.
   */
  template <typename T> void set(T &&value) {
    auto r = std::make_shared<ResultType>(std::forward<T>(value));
    setImpl(std::move(r));
  }

  /**
   * @brief Sets the callback to be called when the result is available.
   *
   * @param cb Client's callback.
   */
  void then(std::function<void(std::shared_ptr<ResultType>)> cb);

  /**
   * @brief Returns a shared pointer to the result.
   *
   * @param timeout The max timeout to block for the result.
   *
   * @return Result value. nullptr if timeout is triggered.
   *
   * @note
   * - If the timeout argument is empty, then the function will block for the
   *   max possible timeout.
   */
  std::shared_ptr<ResultType>
  getPtr(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

  /**
   * @brief Returns the result.
   *
   * @param timeout The max timeout to block for the result.
   *
   * @return Result value as optional. The optional will be invalid in case the
   *         timeout is triggered.
   *
   * @note
   * - If the timeout argument is empty, then the function will block for the
   *   max possible timeout.
   */
  std::optional<ResultType>
  get(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

  /**
   * @brief Delete copy and move semantics.
   */
  FutureResult(const FutureResult &) = delete;
  FutureResult &operator=(const FutureResult &) = delete;
  FutureResult(FutureResult &&) = delete;
  FutureResult &operator=(FutureResult &&) = delete;

private:
  /**
   * @brief Forward declaration for the implementation class.
   */
  class Impl;

  /**
   * @brief Unique pointer to the implementation class.
   */
  std::unique_ptr<Impl> impl_;

  /**
   * @brief Sets the result.
   *
   * @param value Value of the result.
   */
  void setImpl(std::shared_ptr<void> value);
}; // class FutureResult

} // namespace helios::core

#include "future_result.inl"
