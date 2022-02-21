
#include <chrono>
#include <gtest/gtest.h>
#include "concurrency/messagebuffer.h"


using namespace std::chrono_literals;

TEST(messagebuffers, TestQueueUpDown) {

    workshop::messagebuffer<int> bf{10};

    bf.write(1);
    bf.write(2);
    bf.write(3);

    ASSERT_EQ(bf.read(), 1);
    ASSERT_EQ(bf.read(), 2);
    ASSERT_EQ(bf.read(), 3);

    ASSERT_EQ(bf.size(), 0);
}

TEST(messagebuffers, TestWaitRead) {

    workshop::messagebuffer<int> bf{10};

    constexpr auto value = 4;

    std::thread threadReader([&]() {
        auto element = bf.read();

        ASSERT_EQ(element, value);
    });

    std::thread threadWriter([&]() {
        std::this_thread::sleep_for(400ms);
        bf.write(value);
    });

    threadReader.join();
    threadWriter.join();
}

TEST(messagebuffers, TestWaitWrite) {

    workshop::messagebuffer<int> bf{1};

    constexpr auto value = 4;
    constexpr auto readTimeout = 200ms;

    std::thread threadReader([&]() {

        std::this_thread::sleep_for(readTimeout);
        auto element = bf.read();

        ASSERT_EQ(element, value);
    });

    std::thread threadWriter([&]() {
        bf.write(value);
        ASSERT_TRUE(bf.write(2 * value, 2 * readTimeout));
    });

    threadReader.join();
    threadWriter.join();
}
