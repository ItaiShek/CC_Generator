add_library(imgui STATIC Dependencies/imgui-1.89.9/imgui.cpp Dependencies/imgui-1.89.9/imgui_draw.cpp Dependencies/imgui-1.89.9/imgui_widgets.cpp Dependencies/imgui-1.89.9/imgui_tables.cpp Dependencies/imgui-1.89.9/imgui_impl_glfw.cpp Dependencies/imgui-1.89.9/imgui_impl_opengl3.cpp Dependencies/imgui-1.89.9/misc/cpp/imgui_stdlib.cpp)
target_include_directories(imgui PUBLIC Dependencies/imgui-1.89.9)
target_include_directories(imgui PUBLIC Dependencies/imgui-1.89.9/misc/cpp)

add_library(imguifiledialog STATIC Dependencies/ImGuiFileDialog/ImGuiFileDialog.cpp)
target_include_directories(imguifiledialog PUBLIC Dependencies/ImGuiFileDialog Dependencies/imgui-1.89.9)
target_link_libraries(imgui PUBLIC imguifiledialog)


if(WIN32)
    add_definitions(-D_GLFW_WIN32)
    add_library(glfw STATIC Dependencies/glfw-3.3.8/src/context.c Dependencies/glfw-3.3.8/src/egl_context.c Dependencies/glfw-3.3.8/src/init.c Dependencies/glfw-3.3.8/src/input.c Dependencies/glfw-3.3.8/src/monitor.c Dependencies/glfw-3.3.8/src/osmesa_context.c Dependencies/glfw-3.3.8/src/vulkan.c Dependencies/glfw-3.3.8/src/wgl_context.c Dependencies/glfw-3.3.8/src/win32_init.c Dependencies/glfw-3.3.8/src/win32_joystick.c Dependencies/glfw-3.3.8/src/win32_monitor.c Dependencies/glfw-3.3.8/src/win32_thread.c Dependencies/glfw-3.3.8/src/win32_time.c Dependencies/glfw-3.3.8/src/win32_window.c Dependencies/glfw-3.3.8/src/window.c)
    target_link_libraries(glfw PRIVATE opengl32)
elseif(UNIX)
    add_definitions(-D_GLFW_X11)
    add_library(glfw STATIC Dependencies/glfw-3.3.8/src/context.c Dependencies/glfw-3.3.8/src/egl_context.c Dependencies/glfw-3.3.8/src/glx_context.c Dependencies/glfw-3.3.8/src/init.c Dependencies/glfw-3.3.8/src/input.c Dependencies/glfw-3.3.8/src/linux_joystick.c Dependencies/glfw-3.3.8/src/monitor.c Dependencies/glfw-3.3.8/src/osmesa_context.c Dependencies/glfw-3.3.8/src/posix_thread.c Dependencies/glfw-3.3.8/src/posix_time.c Dependencies/glfw-3.3.8/src/vulkan.c  Dependencies/glfw-3.3.8/src/window.c Dependencies/glfw-3.3.8/src/x11_init.c Dependencies/glfw-3.3.8/src/x11_monitor.c Dependencies/glfw-3.3.8/src/x11_window.c Dependencies/glfw-3.3.8/src/xkb_unicode.c)
    target_link_libraries(glfw PRIVATE dl GL X11)
endif()

target_include_directories(glfw PUBLIC Dependencies/glfw-3.3.8/include)
target_link_libraries(imgui PUBLIC glfw)