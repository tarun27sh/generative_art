cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (CPP_GENERATIVE_ART)

include(ExternalProject)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

ExternalProject_Add(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.x
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

add_executable (generative_art  generative_art.cpp)
add_dependencies(generative_art spdlog)
#target_link_libraries(generative_art spdlog)

set(CMAKE_CXX_FLAGS "-std=c++2a -fext-numeric-literals")
