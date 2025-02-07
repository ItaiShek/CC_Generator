cmake_minimum_required(VERSION 3.16...3.27)
project(CC_Generator VERSION 2.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_library(api STATIC src/API/DB_API.cpp src/API/Card.cpp src/API/Date.cpp)
target_include_directories(api PUBLIC include/API extern/sqlite)
add_library(sqlite STATIC extern/sqlite/sqlite3.c)

if(UNIX)
    target_link_libraries(sqlite PRIVATE sqlite3)
endif()

# Console
if (WIN32)
    set(CURSES_SOURCES
        extern/PDCurses/pdcurses/addch.c
        extern/PDCurses/pdcurses/addchstr.c
        extern/PDCurses/pdcurses/addstr.c
        extern/PDCurses/pdcurses/attr.c
        extern/PDCurses/pdcurses/beep.c
        extern/PDCurses/pdcurses/bkgd.c
        extern/PDCurses/pdcurses/border.c
        extern/PDCurses/pdcurses/clear.c
        extern/PDCurses/pdcurses/color.c
        extern/PDCurses/pdcurses/debug.c
        extern/PDCurses/pdcurses/delch.c
        extern/PDCurses/pdcurses/deleteln.c
        extern/PDCurses/pdcurses/getch.c
        extern/PDCurses/pdcurses/getstr.c
        extern/PDCurses/pdcurses/getyx.c
        extern/PDCurses/pdcurses/inch.c
        extern/PDCurses/pdcurses/inchstr.c
        extern/PDCurses/pdcurses/initscr.c
        extern/PDCurses/pdcurses/inopts.c
        extern/PDCurses/pdcurses/insch.c
        extern/PDCurses/pdcurses/insstr.c
        extern/PDCurses/pdcurses/instr.c
        extern/PDCurses/pdcurses/kernel.c
        extern/PDCurses/pdcurses/keyname.c
        extern/PDCurses/pdcurses/mouse.c
        extern/PDCurses/pdcurses/move.c
        extern/PDCurses/pdcurses/outopts.c
        extern/PDCurses/pdcurses/overlay.c
        extern/PDCurses/pdcurses/pad.c
        extern/PDCurses/pdcurses/panel.c
        extern/PDCurses/pdcurses/printw.c
        extern/PDCurses/pdcurses/README.md
        extern/PDCurses/pdcurses/refresh.c
        extern/PDCurses/pdcurses/scanw.c
        extern/PDCurses/pdcurses/scroll.c
        extern/PDCurses/pdcurses/scr_dump.c
        extern/PDCurses/pdcurses/slk.c
        extern/PDCurses/pdcurses/termattr.c
        extern/PDCurses/pdcurses/touch.c
        extern/PDCurses/pdcurses/util.c
        extern/PDCurses/pdcurses/window.c
        extern/PDCurses/wincon/pdcclip.c
        extern/PDCurses/wincon/pdcdisp.c
        extern/PDCurses/wincon/pdcgetsc.c
        extern/PDCurses/wincon/pdckbd.c
        extern/PDCurses/wincon/pdcscrn.c
        extern/PDCurses/wincon/pdcsetsc.c
        extern/PDCurses/wincon/pdcutil.c
    )

    add_library(ncurses STATIC ${CURSES_SOURCES})
    target_include_directories(ncurses PUBLIC extern/PDCurses extern/PDCurses/wincon)
endif()

add_library(console STATIC src/Console/Console.cpp)
target_include_directories(console PUBLIC include/Console)
target_link_libraries(console PRIVATE api ncurses)


add_executable(CC_Generator_Console src/CC_Generator/console_main.cpp)
target_link_libraries(CC_Generator_Console PRIVATE api console sqlite ncurses)
target_compile_features(CC_Generator_Console PUBLIC cxx_std_20)

# GUI

add_library(imgui STATIC
                extern/DearImGui/imgui.cpp
                extern/DearImGui/imgui_draw.cpp
                extern/DearImGui/imgui_widgets.cpp
                extern/DearImGui/imgui_tables.cpp
                extern/DearImGui/backends/imgui_impl_glfw.cpp
                extern/DearImGui/backends/imgui_impl_opengl3.cpp
                extern/DearImGui/misc/cpp/imgui_stdlib.cpp
            )


target_include_directories(imgui PUBLIC extern/DearImGui)
target_include_directories(imgui PUBLIC extern/DearImGui/misc/cpp)

add_library(imguifiledialog STATIC extern/ImGuiFileDialog/ImGuiFileDialog.cpp)
target_include_directories(imguifiledialog PUBLIC extern/ImGuiFileDialog extern/DearImGui)
target_link_libraries(imgui PUBLIC imguifiledialog)


if(WIN32)
    add_definitions(-D_GLFW_WIN32)
    add_library(glfw STATIC
        extern/glfw/src/context.c
        extern/glfw/src/init.c
        extern/glfw/src/input.c
        extern/glfw/src/monitor.c
        extern/glfw/src/vulkan.c
        extern/glfw/src/window.c
        extern/glfw/src/platform.c
        extern/glfw/src/null_init.c
        extern/glfw/src/null_monitor.c
        extern/glfw/src/null_window.c
        extern/glfw/src/win32_module.c
        extern/glfw/src/null_joystick.c
        extern/glfw/src/win32_init.c
        extern/glfw/src/win32_joystick.c
        extern/glfw/src/win32_monitor.c
        extern/glfw/src/win32_time.c
        extern/glfw/src/win32_thread.c
        extern/glfw/src/win32_window.c
        extern/glfw/src/wgl_context.c
        extern/glfw/src/egl_context.c
        extern/glfw/src/osmesa_context.c
    )

    target_link_libraries(glfw PRIVATE opengl32)
elseif(UNIX)
    add_definitions(-D_GLFW_X11)
    add_library(glfw STATIC
    extern/glfw/src/context.c
    extern/glfw/src/init.c
    extern/glfw/src/input.c
    extern/glfw/src/monitor.c
    extern/glfw/src/vulkan.c
    extern/glfw/src/window.c
    extern/glfw/src/platform.c
    extern/glfw/src/null_init.c
    extern/glfw/src/null_monitor.c
    extern/glfw/src/null_window.c
    extern/glfw/src/x11_init.c
    extern/glfw/src/x11_monitor.c
    extern/glfw/src/x11_window.c
    extern/glfw/src/xkb_unicode.c
    extern/glfw/src/glx_context.c
    extern/glfw/src/egl_context.c
    extern/glfw/src/osmesa_context.c
    extern/glfw/src/linux_joystick.c
    extern/glfw/src/posix_time.c
    extern/glfw/src/posix_thread.c
    extern/glfw/src/posix_module.c
    extern/glfw/src/posix_poll.c
    extern/glfw/src/null_joystick.c
)
target_link_libraries(glfw PRIVATE dl GL X11)


    target_link_libraries(glfw PRIVATE dl GL X11)
endif()

target_include_directories(glfw PUBLIC extern/glfw/include)
target_link_libraries(imgui PUBLIC glfw)

add_library(gui STATIC src/GUI/GUI.cpp)
target_include_directories(gui PUBLIC include/GUI extern/DearImGui/backends)
target_link_libraries(gui PUBLIC api imgui)
if (WIN32)
    add_executable(CC_Generator_GUI WIN32 src/CC_Generator/gui_main.cpp)
elseif(UNIX)
    add_executable(CC_Generator_GUI src/CC_Generator/gui_main.cpp)
endif()

target_link_libraries(CC_Generator_GUI PRIVATE api gui sqlite)
target_compile_features(CC_Generator_GUI PUBLIC cxx_std_20)