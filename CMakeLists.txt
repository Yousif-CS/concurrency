cmake_minimum_required(VERSION 3.15)
project(Concurrency)


# This is for resolving header file includes using text editors like
# vim/emacs/vscode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We will be using c++17
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(thirdparty)
add_subdirectory(include)

option(BUILD_TESTS "Build library tests" ON)
option(BUILD_EXAMPLES "Build example executables" ON)

if (BUILD_TESTS)
    message(STATUS "Building tests...")
    add_subdirectory(tests)
endif()

if (BUILD_EXAMPLES)
    message(STATUS "Building examples...")
    add_subdirectory(examples)
endif()

install(FILES cmake/concurrency-config.cmake DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/concurrency/cmake)

install(EXPORT ${CMAKE_PROJECT_NAME} DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/concurrency/cmake)
