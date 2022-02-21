project(${CMAKE_PROJECT_NAME})


find_package(Boost REQUIRED)

add_library(concurrency INTERFACE
    ${CONCURRENCY_INTERFACE_FILES})


set(CONCURRENCY_INTERFACE_FILES
    concurrency/threadpool.h
    concurrency/workerthread.h
    concurrency/messagebuffer.h)

target_link_libraries(concurrency INTERFACE
    Boost::boost pthread)


target_include_directories(concurrency
    INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)


install(TARGETS concurrency EXPORT ${CMAKE_PROJECT_NAME} DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/concurrency/cmake)

install(FILES ${CONCURRENCY_INTERFACE_FILES} DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/concurrency)
