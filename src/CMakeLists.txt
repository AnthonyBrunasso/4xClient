file(GLOB Sources "*.cpp" "*.c" "../include/*.h")

add_subdirectory("e4x")
add_subdirectory("4xSimulation")

include_directories("4xSimulation/include")
include_directories("e4x/include")

add_executable(4xclient ${Sources})

target_link_libraries(4xclient 4xlib)
target_link_libraries(4xclient e4x)
