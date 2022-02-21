project(${CMAKE_PROJECT_NAME})


include(googletest.cmake)
enable_testing()

set(TEST_FILES
    messagebuffer_test.cpp
    workerthread_test.cpp
    threadpool_test.cpp)


add_executable(tests ${TEST_FILES})

target_link_libraries(tests PRIVATE concurrency gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

