file(GLOB Sources "*.cpp" "*.c")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("glfw-3.2")
find_package(OpenGL REQUIRED)

add_executable(4xClient ${Sources})

include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(4xClient glfw)
target_link_libraries(4xClient ${OPENGL_LIBRARIES})
