#pragma once
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <string>
#include <cstdint>

extern bool g_emscripten_save_db_flag;
extern bool g_emscripten_open_flag;
extern bool g_emscripten_generate_flag;

/**
 * @brief Retrieves a selected file path from a JavaScript global variable.
 *
 * This function queries the specified JavaScript variable (e.g., set by a file input dialog)
 * and returns its value as a C++ string. If the variable is undefined or null, an empty string is returned.
 *
 * @param var The name of the global JavaScript variable holding the file path (without `window.` prefix).
 * @return The file path as a std::string, or an empty string if not set.
 */
std::string get_selected_file_path_from_js(const std::string &var)
{
    std::string str = "window." + var + " ? window." + var + " : ''";
    const char *path = emscripten_run_script_string(str.c_str());
    return std::string(path);
}

/**
 * @brief Opens a file dialog in the browser and loads the selected file into Emscripten's virtual file system (VFS).
 *
 * This function triggers a file input dialog using JavaScript and, upon file selection, reads the file into memory
 * and writes it to the VFS under the `/tmp/` directory. It also sets a global JavaScript variable `window.selectedFilePath`
 * and updates a shared C++ boolean flag (`g_emscripten_open_flag`) to signal completion.
 *
 * @param filter The file type filter (e.g., ".db") to restrict selectable files in the dialog.
 */
void open_file_dialog(const std::string &filter)
{
    uintptr_t flag_ptr = (uintptr_t)&g_emscripten_open_flag;
    const char *filter_cstr = filter.c_str(); // ensure pointer remains valid

    EM_ASM_({
        // Reset flag initially (0 = false)
        // Write 0 to the C++ bool flag memory
        HEAPU8[$0] = 0;

        let input = document.createElement('input');
        window.selectedFilePath = null;
        input.type = 'file';
        // input.accept = '.db';
        input.accept = UTF8ToString($1);

        input.onchange = e => {
            let file = e.target.files[0];
            let reader = new FileReader();

            reader.onload = function () {
                let data = new Uint8Array(reader.result);
                let filename = "/tmp/" + file.name;

                FS.writeFile(filename, data);
                console.log("File written to VFS:", filename);
                window.selectedFilePath = filename;
                // Set C++ flag to true by writing 1 into the memory location
                HEAPU8[$0] = 1;
            };

            reader.readAsArrayBuffer(file);
        };

        input.click(); }, flag_ptr, filter_cstr);
}

/**
 * @brief Triggers a download of a file from Emscripten's virtual file system (VFS) to the user's local machine.
 *
 * This function reads the specified file from the VFS, creates a downloadable Blob in the browser,
 * and simulates a click to start the download. The download is named according to `download_file`.
 * After triggering the download, it sets the referenced C++ flag to `false` (0) to signal completion.
 *
 * @param vfs_path Path to the file in Emscripten's virtual file system.
 * @param download_file Desired name for the downloaded file.
 * @param flag Reference to a boolean flag updated after the download is triggered.
 */
void download_file(const std::string &vfs_path, const std::string &download_file, bool &flag)
{
    uintptr_t flag_ptr = reinterpret_cast<uintptr_t>(&flag);

    EM_ASM_({
        const path = UTF8ToString($0);
        const downloadName = UTF8ToString($1);
        const flagPtr = $2;

        try
        {
            const data = FS.readFile(path);
            const blob = new Blob([data],
                                  { type: 'application/octet-stream' });
            const url = URL.createObjectURL(blob);

            const a = document.createElement('a');
            a.href = url;
            a.download = downloadName;
            a.setAttribute('download', downloadName);
            a.click();

            URL.revokeObjectURL(url);
            console.log("Download triggered:", path);
        }
        catch (e)
        {
            console.error("Failed to download file:", e);
        }

        // Set the C++ flag to false (0)
        HEAPU8[flagPtr] = 0; }, vfs_path.c_str(), download_file.c_str(), flag_ptr);
}

/**
 * @brief Sets the background color of the HTML <html> and <body> elements.
 *
 * This function is implemented in JavaScript via Emscripten and modifies the
 * background color of both the `<html>` and `<body>` elements using the given
 * hexadecimal color code.
 *
 * @param color A C-style string representing the color in hex format (e.g., "#FFFFFF").
 */
EM_JS(void, set_background_color, (const char *color), {
    document.documentElement.style.setProperty('background-color', UTF8ToString(color));
    document.body.style.setProperty('background-color', UTF8ToString(color));
});

/**
 * @brief Sets the webpage background to a dark color.
 *
 * Internally calls `set_background_color()` with a dark color value.
 * Intended for toggling to dark mode.
 */
void set_background_dark_mode()
{
    set_background_color("#0E0E0E");
}

/**
 * @brief Sets the webpage background to a light color.
 *
 * Internally calls `set_background_color()` with a light color value.
 * Intended for toggling to light mode.
 */
void set_background_light_mode()
{
    set_background_color("#F0F0F0");
}

#endif