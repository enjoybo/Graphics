cmake_minimum_required (VERSION 2.6)
project(tutorial)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
)
set(MY_HEADERS
		common/shader.hpp
)
set(MY_SOURCE_FILES
		tutorial02.cpp
		common/shader.cpp
)
add_executable(tutorial ${MY_HEADERS} ${MY_SOURCE_FILES})

target_link_libraries(tutorial glfw)
target_link_libraries(tutorial ${GLEW_LIBRARIES})
target_link_libraries(tutorial ${OPENGL_gl_LIBRARY})

