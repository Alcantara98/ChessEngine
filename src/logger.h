#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace engine
{
/**
 * @brief Initializes file logging for the chess engine.
 *
 * @param log_name Base name for the log file (without path or extension).
 * Writes to `log/<log_name>.log` in the process working directory.
 *
 * @details Creates the `log` directory if needed. Does not write to
 * stdout/stderr so UCI protocol traffic on stdout stays clean.
 *
 * Log lines look like:
 * `[2026-07-11 15:02:30.123] [warning] search_engine.cpp:142: time left: 50ms`
 *
 * Use the SPDLOG_* macros so file and line are captured:
 * `#include <spdlog/spdlog.h>`
 * `SPDLOG_INFO("depth {} score {}", depth, score);`
 * `SPDLOG_WARN("time left: {}ms", remaining_ms);`
 * `SPDLOG_ERROR("bad move: {}", move);`
 */
void init_logging(const std::string &log_name);
} // namespace engine

#endif
