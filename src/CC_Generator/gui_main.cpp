/**
 * @file main.cpp
 * @brief Entry point for the GUI application.
 *
 * This file contains the main function for running the GUI version of the application.
 * It handles platform-specific entry points for both Windows and Linux.
 *
 * On Windows, the `WinMain` function is used as the entry point, while on Linux,
 * the regular `main` function is used.
 *
 * @see GUI.h for the declaration of the gui::run function.
 * @see ProjectInfo_ for version, project URL, and license details.
 */

#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#undef MOUSE_MOVED
#endif

#include "GUI.h"
#include "ProjectInfo_.h"

/**
 * @brief Main function to run the GUI version of the application.
 *
 * On Windows, this function uses the `WinMain` entry point, while on Linux it uses the regular `main`.
 * It initializes the GUI application by calling the `gui::run` function with version, project URL, and license
 * information retrieved from the `ProjectInfo` class.
 *
 * @param hInstance The handle to the application instance (Windows only).
 * @param hPrevInstance The handle to the previous instance (Windows only).
 * @param lpCmdLine Command line arguments (Windows only).
 * @param nCmdShow Controls how the window is shown (Windows only).
 * @param argc The number of command-line arguments (Linux only).
 * @param argv The array of command-line argument strings (Linux only).
 *
 * @return Returns 0 if the application runs successfully.
 */
#if defined(_WIN64) || defined(_WIN32)
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
    // Run the GUI application with the specified project details
    gui::run(ProjectInfo::version, ProjectInfo::project_url, ProjectInfo::license);
    return 0;
}