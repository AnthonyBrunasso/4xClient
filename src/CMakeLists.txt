file(GLOB Sources "*.cpp" "*.c" "../include/*.h")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("glfw-3.2")
add_subdirectory("4xSimulation")
add_subdirectory("socket")
include_directories("./")
include_directories("4xSimulation/include")
include_directories("socket/include")
find_package(OpenGL REQUIRED)

add_executable(4xclient ${Sources})

include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(4xclient glfw)
target_link_libraries(4xclient ${OPENGL_LIBRARIES})
target_link_libraries(4xclient 4xlib)
target_link_libraries(4xclient libxsocket)

IF(WIN32)
  target_link_libraries(4xclient ws2_32)
ENDIF(WIN32)

