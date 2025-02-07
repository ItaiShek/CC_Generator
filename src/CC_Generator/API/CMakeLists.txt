add_library(sqlite STATIC Dependencies/sqlite/sqlite3.c)
target_include_directories(sqlite PRIVATE Dependencies/sqlite)

if(UNIX)
    target_link_libraries(sqlite PRIVATE sqlite3)
endif()