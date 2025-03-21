#include "cross_plat_functions.h"

namespace engine
{
auto inputAvailable() -> bool
{
#ifdef _WIN32
  if (_kbhit() != 0)
  {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events;
    GetNumberOfConsoleInputEvents(hStdin, &events);
    return events > 0;
  }
  return false;
#else
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);

  struct timeval timeout = {0, 0}; // No blocking, just check
  return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &timeout) > 0;
#endif
}
} // namespace engine