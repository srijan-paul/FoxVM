cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(foxvm)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*pp")
add_executable(${PROJECT_NAME} ${SOURCES})
set(CMAKE_CXX_STANDARD 20)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
