project(${CMAKE_PROJECT_NAME})

add_executable(main main.cpp)

target_link_libraries(main PRIVATE concurrency)

