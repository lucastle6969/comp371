cmake_minimum_required(VERSION 3.8)
project(SkeletonCode)

add_definitions(-Wno-deprecated-declarations -DCMAKE_BUILD_TYPE=Release -DGLM_FORCE_RADIANS)

#Set the correct output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

## Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")  ## Strip binary
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
        main.cpp
        objloader.cpp
        objloader.hpp)


set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Configuring packages

#Find GLM
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
#MESSAGE(STATUS "GLM: " ${GLM_INCLUDE_DIRS})

#Find GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

#Find OPENGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#Find GLFW
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

add_executable(SkeletonCode ${SOURCE_FILES})
target_link_libraries(SkeletonCode
                        ${GLEW_LIBRARY}
                        ${OPENGL_LIBRARIES}
                        ${GLFW_LIBRARIES})