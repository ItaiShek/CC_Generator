cmake_minimum_required(VERSION 3.16...3.27)
project(CC_Generator VERSION 2.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_library(api STATIC src/API/DB_API.cpp src/API/Card.cpp src/API/Date.cpp)
target_include_directories(api PUBLIC include/API extern/sqlite)
add_library(sqlite STATIC extern/sqlite/sqlite3.c)

target_link_libraries(sqlite PRIVATE sqlite3)

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