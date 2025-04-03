#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

namespace engine
{
/**
 * @brief Checks if input is available.
 *
 * @details This function is platform dependent:
 * - On Windows, it uses `_kbhit` from `<conio.h>`.
 * - On Unix-like systems, it uses `select` from `<sys/select.h>`.
 *
 * @return True if input is available, false otherwise.
 */
auto inputAvailable() -> bool;
} // namespace engine
