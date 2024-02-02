#include "DB_API.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <limits.h>

#if defined(_WIN64) || defined(_WIN32)
#include <Windows.h>
#define OPEN_DYNAMIC(X) LoadLibraryA(X)
#define GET_PROC_ADDR(X, Y) GetProcAddress((HMODULE)X, Y)
#define CLOSE_HANDLE(X) FreeLibrary((HMODULE)X)
constexpr char console_file[] = "Console.dll";
constexpr char gui_file[] = "GUI.dll";
#else
#include <dlfcn.h>
#include <unistd.h>
#define OPEN_DYNAMIC(X) dlopen(X, RTLD_LAZY)
#define GET_PROC_ADDR(X, Y) dlsym(X, Y);
#define CLOSE_HANDLE(X) dlclose(X)
constexpr char console_file[] = "Console.so";
constexpr char gui_file[] = "GUI.so";
#endif

typedef void (*run_gen)(const std::string&, const std::string&, const std::string&);

const std::string version{ "2.0.0" };
const std::string project_url{ "https://github.com/ItaiShek/CC_Generator" };
const std::string license{ "AGPL V3" };

/**
 * @brief Get the directory path of the currently executing program.
 *
 * This function retrieves the directory path of the currently executing program.
 *
 * @return The directory path of the currently executing program.
 */
std::string get_exec_path()
{
    std::string executable_path;
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    executable_path = buffer;
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        executable_path = buffer;
    }
#endif
    // Extract directory part of the path
    size_t found = executable_path.find_last_of("/\\");
    if (found != std::string::npos)
    {
        executable_path = executable_path.substr(0, found);
    }
    return executable_path;
}

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
    std::string full_path = get_exec_path();
    if (full_path.empty())
    {
        std::cerr << "Failed to retrieve executable path." << std::endl;
        return 1;
    }

    std::string dynamic_file = full_path;

    void* lib_handle{};
    bool use_console_mode = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--console") == 0)
        {
            use_console_mode = true;
            break;
        }
    }

    if (use_console_mode)
    {
        // Start the console application
        dynamic_file = dynamic_file + '/' + console_file;
    }
    else
    {
        // Start the GUI application        
        dynamic_file = dynamic_file + '/' + gui_file;
    }

    lib_handle = OPEN_DYNAMIC(dynamic_file.c_str());
    if (lib_handle == NULL)
    {
        std::cerr << "Couldn't load: " << dynamic_file << std::endl;
        return 1;
    }

    run_gen run_function{ nullptr };
    run_function = (run_gen)GET_PROC_ADDR(lib_handle, "run");

    if (run_function == NULL)
    {
        std::cerr << "Function not found\n";
        CLOSE_HANDLE(lib_handle);
        return 1;
    }

    run_function(version, project_url, license);
    CLOSE_HANDLE(lib_handle);

    return 0;
}