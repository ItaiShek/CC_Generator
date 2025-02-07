#pragma once
#include <string>

namespace gui
{
    /**
     * @brief Function signature for the GUI application entry point.
     *
     * This function is the entry point for the GUI application and is meant
     * to be called from external code to start the application.
     *
     * @param version A string representing the version information.
     * @param url A string containing the URL information.
     * @param license A string containing the license information.
     */
    void run(const std::string& version, const std::string& url, const std::string& license);
}