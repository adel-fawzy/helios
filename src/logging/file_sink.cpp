#include "file_sink.hpp"

namespace helios::logging {

FileSink::FileSink(std::unique_ptr<IFile> file,
                   std::shared_ptr<core::IEventQueue> eventQueue)
    : file_{std::move(file)}, Module{eventQueue} {}

void FileSink::writeImpl(std::string msg) {
  add([this, msg = std::move(msg)] { file_->append(msg); });
}

} // namespace helios::logging
