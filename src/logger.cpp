#include "logger.h"

#include <filesystem>
#include <memory>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace engine
{
namespace
{
const std::string LOG_DIR = "log";
} // namespace

void init_logging(const std::string &log_name)
{
  if (spdlog::get(log_name) != nullptr)
  {
    return;
  }

  std::filesystem::create_directories(LOG_DIR);

  const std::string log_file_path = LOG_DIR + "/" + log_name + ".log";
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      log_file_path, false);
  auto logger = std::make_shared<spdlog::logger>(log_name, file_sink);

  // date + time.ms + level + file:line — file:line is easy to jump to in the editor
  logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %s:%#: %v");
  logger->set_level(spdlog::level::info);
  logger->flush_on(spdlog::level::info);

  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger);

  SPDLOG_INFO("=== LiYing Engine log started ({}) ===", log_name);
}
} // namespace engine
