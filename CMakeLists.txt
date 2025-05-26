cmake_minimum_required(VERSION 3.16...3.27)
project(CC_Generator VERSION 2.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_library(api STATIC src/API/DB_API.cpp src/API/Card.cpp src/API/Date.cpp)
target_include_directories(api PUBLIC include/API extern/sqlite)
add_library(sqlite STATIC extern/sqlite/sqlite3.c)

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


add_library(gui STATIC src/GUI/GUI.cpp)
target_include_directories(gui PUBLIC include/GUI extern/DearImGui/backends)
target_link_libraries(gui PUBLIC api imgui)
target_link_libraries(imgui PUBLIC glfw)

# Emscripten
set(CMAKE_EXECUTABLE_SUFFIX ".html")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
  -sUSE_GLFW=3 \
  -sUSE_WEBGL2=1 \
  -sALLOW_MEMORY_GROWTH=1 \
  -sASYNCIFY \
  -sFORCE_FILESYSTEM=1 \
  --preload-file Cards.db \
  --shell-file \"${CMAKE_SOURCE_DIR}/shell.html\" \
  -sEXPORTED_FUNCTIONS=['_main'] \
  -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']")

add_executable(CC_Generator_GUI src/CC_Generator/gui_main.cpp)
set_target_properties(CC_Generator_GUI PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/CC_Generator
)
target_link_libraries(CC_Generator_GUI PRIVATE api gui sqlite)
target_compile_features(CC_Generator_GUI PUBLIC cxx_std_20)
add_custom_command(TARGET CC_Generator_GUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
            $<TARGET_FILE:CC_Generator_GUI>
            ${CMAKE_BINARY_DIR}/CC_Generator/index.html
)