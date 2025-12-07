#pragma once

#include <condition_variable>
#include <mutex>

namespace helios::core {

template <typename ResultType> class FutureResult<ResultType>::Impl {
public:
  /**
   * @brief Value of the result.
   */
  std::shared_ptr<void> result_;

  /**
   * @brief Protects this class.
   */
  std::mutex mtx_;

  /**
   * @brief Synchronizes with calling thread.
   */
  std::condition_variable cv_;

  /**
   * @brief Callback to receive the result asynchronously.
   */
  std::function<void(std::shared_ptr<ResultType>)> cb_;
}; // class FutureResult<ResultType>::Impl

template <typename ResultType>
FutureResult<ResultType>::FutureResult() : impl_(std::make_unique<Impl>()) {}

template <typename ResultType>
FutureResult<ResultType>::~FutureResult() = default;

template <typename ResultType>
void FutureResult<ResultType>::setImpl(std::shared_ptr<void> value) {
  std::function<void(std::shared_ptr<ResultType>)> copy;
  {
    std::lock_guard<std::mutex> lock(impl_->mtx_);
    impl_->result_ = value; // Store the result
    copy = impl_->cb_;
  }

  impl_->cv_.notify_one(); // Notify other threads that the result is ready

  if (copy)
    // Call the client's callback with the new result
    copy(std::static_pointer_cast<ResultType>(value));
}

template <typename ResultType>
void FutureResult<ResultType>::then(
    std::function<void(std::shared_ptr<ResultType>)> cb) {
  {
    std::lock_guard<std::mutex> lock(impl_->mtx_);

    if (!impl_->result_) {
      impl_->cb_ = std::move(cb);
      return;
    }
  }
  cb(std::static_pointer_cast<ResultType>(impl_->result_));
}

template <typename ResultType>
std::shared_ptr<ResultType>
FutureResult<ResultType>::getPtr(std::chrono::milliseconds timeout) {
  std::unique_lock<std::mutex> lock(impl_->mtx_);
  if (timeout == std::chrono::milliseconds::max()) {
    impl_->cv_.wait(lock, [this] { return impl_->result_ != nullptr; });
  } else {
    impl_->cv_.wait_for(lock, timeout,
                        [this] { return impl_->result_ != nullptr; });
  }
  return std::static_pointer_cast<ResultType>(impl_->result_);
}

template <typename ResultType>
std::optional<ResultType>
FutureResult<ResultType>::get(std::chrono::milliseconds timeout) {
  std::unique_lock<std::mutex> lock(impl_->mtx_);
  std::optional<ResultType> res;
  if (timeout == std::chrono::milliseconds::max()) {
    impl_->cv_.wait(lock, [this] { return impl_->result_ != nullptr; });
    res = *std::static_pointer_cast<ResultType>(impl_->result_);
  } else {
    impl_->cv_.wait_for(lock, timeout,
                        [this] { return impl_->result_ != nullptr; });
  }
  return res;
}

} // namespace helios::core
