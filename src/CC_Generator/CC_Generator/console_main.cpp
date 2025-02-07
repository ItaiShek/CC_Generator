#include "DB_API.h"
#include "Console.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <limits.h>

#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#endif

const std::string version{ "2.0.1" };
const std::string project_url{ "https://github.com/ItaiShek/CC_Generator" };
const std::string license{ "AGPL V3" };

/**
 * @brief The main entry point for the program.
 *
 * This function retrieves the executable path, determines whether to start
 * the console or GUI application, and dynamically loads the corresponding
 * shared library to execute the `run` function.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 *
 * @return 0 on successful execution, 1 otherwise.
 */
int main(int argc, char* argv[])
{
    console::run(version, project_url, license);
    return 0;
}