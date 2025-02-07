cmake_minimum_required(VERSION 3.5.0)
project(CC_Generator VERSION 2.0.1 LANGUAGES C CXX)

add_subdirectory(API)
add_subdirectory(Console)
add_subdirectory(GUI)

add_library(api STATIC ${CMAKE_SOURCE_DIR}/API/DB_API.cpp ${CMAKE_SOURCE_DIR}/API/Card.cpp)
target_include_directories(api PUBLIC ${CMAKE_SOURCE_DIR}/API)

# Console
add_library(console STATIC ${CMAKE_SOURCE_DIR}/Console/Console.cpp)
target_include_directories(console PUBLIC ${CMAKE_SOURCE_DIR}/Console)
target_link_libraries(console PRIVATE api ncurses)

add_executable(CC_Generator_Console ${CMAKE_SOURCE_DIR}/CC_Generator/console_main.cpp)
target_link_libraries(CC_Generator_Console PRIVATE api console sqlite ncurses)
target_compile_features(CC_Generator_Console PUBLIC cxx_std_14)


# GUI
add_library(gui STATIC ${CMAKE_SOURCE_DIR}/GUI/GUI.cpp)
target_include_directories(gui PUBLIC ${CMAKE_SOURCE_DIR}/GUI)
target_link_libraries(gui PUBLIC api imgui)
if (WIN32)
    add_executable(CC_Generator_GUI WIN32 ${CMAKE_SOURCE_DIR}/CC_Generator/gui_main.cpp)
elseif(UNIX)
    add_executable(CC_Generator_GUI ${CMAKE_SOURCE_DIR}/CC_Generator/gui_main.cpp)
endif()

target_link_libraries(CC_Generator_GUI PRIVATE api gui sqlite)
target_compile_features(CC_Generator_GUI PUBLIC cxx_std_14)