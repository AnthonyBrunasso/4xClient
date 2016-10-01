file(GLOB Sources "*.cpp" "*.c" "../include/*.h")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("glfw-3.2")
add_subdirectory("socket")

include_directories("./")
include_directories("socket/include")

find_package(OpenGL REQUIRED)

add_library(e4x ${Sources})

include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(e4x glfw)
target_link_libraries(e4x ${OPENGL_LIBRARIES})
target_link_libraries(e4x libxsocket)

IF(WIN32)
  target_link_libraries(e4x ws2_32)
ENDIF(WIN32)

