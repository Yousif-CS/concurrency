
#include <iostream>
#include <fstream>
#include <functional>
#include <chrono>
#include <atomic>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <gtest/gtest.h>


#include "concurrency/threadpool.h"

using namespace std::chrono_literals;

/**
 * Test incrementing a variable many times
 */
TEST(threadpool, incrementTestFullThreads) {

    constexpr auto numTasks = 100;
    workshop::threadpool<std::function<void()>> pool(numTasks);
    std::mutex mtx;
    int counter = 0;

    auto task = [&]() {
        std::this_thread::sleep_for(100ms);
        std::unique_lock<std::mutex>lock(mtx);
        counter++;
    };

    for(size_t i = 0; i < numTasks; i++) {
        pool.postTask(task);
    }
    pool.sync();
    //ASSERT_EQ(counter.load(), numTasks);
}


/**
 * Test incrementing a variable many times
 */
TEST(threadpool, incrementTestOneThread) {

    constexpr auto numTasks = 100;
    workshop::threadpool<std::function<void()>> pool(numTasks, 1);
    std::mutex mtx;
    int counter = 0;

    auto task = [&](){
        std::this_thread::sleep_for(100ms);
        std::unique_lock<std::mutex>lock(mtx);
        counter++;
    };

    for(size_t i = 0; i < numTasks; i++) {
        pool.postTask(task);
    }

    pool.sync();
    //ASSERT_EQ(counter.load(), numTasks);
}

/**
 * Test incrementing a variable many times
 */
TEST(threadpool, incrementTestNoThreadPool) {

    constexpr auto numTasks = 100;

    int ctr = 0;

    for(size_t i = 0; i < numTasks; i++) {
        std::this_thread::sleep_for(100ms);
        ctr++;
    }

    // ASSERT_EQ(counter.load(), numTasks);
}
