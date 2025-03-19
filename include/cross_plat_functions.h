#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

/**
 * @brief Checks if input is available.
 *
 * @details This function is platform dependent.
 *
 * @return True if input is available, false otherwise.
 */
auto inputAvailable() -> bool;
