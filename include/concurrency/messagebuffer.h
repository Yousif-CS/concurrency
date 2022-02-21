/* *
 * Written by Yousif on 30th Jan 2021.
 * A Message buffer; A thread safe buffer for that supports multiple readers and writers
 * */

#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <optional>

#include <boost/circular_buffer.hpp>

namespace workshop {

    using duration = std::chrono::duration<int,std::milli>;
    static constexpr auto DEFAULT_CAPACITY = 10;

    template <typename T>
    class messagebuffer
    {
    public:
        messagebuffer(size_t capacity)
        : mCapacity(capacity), mBuffer(capacity)
        {}

        messagebuffer()
        : messagebuffer(DEFAULT_CAPACITY)
        {}

        /* *
         * Read a message from the message queue. Blocks if there are
         * no messages yet.
         * @return an optional object containing T, depending on whether unblock
         * was called or not
         */

        std::optional<T> read() {
            std::unique_lock<std::mutex> lock(mMutex);

            if (!mKill && mBuffer.size() == 0) {
                mCvRead.wait(lock, [&](){ return mKill || mBuffer.size() > 0; });

                if (mKill) return {};
            }

            auto element = mBuffer.front();
            mBuffer.pop_front();

            if (mBuffer.size() == mCapacity - 1) {
                mCvWrite.notify_one();
            }

            if (mBuffer.empty()) {
                mCvEmpty.notify_all();
            }

            return element;
        }

        /* *
         * Write a message to the buffer. Blocks if the buffer is full
         * @param message The message to be written
         */
        void write(T message) {
            std::unique_lock<std::mutex> lock(mMutex);

            // Calling this function means you want
            // to restore the buffer into a usable state
            if (mKill) mKill = false;

            if (mBuffer.size() == mCapacity) {
                mCvWrite.wait(lock, [this]() { return mBuffer.size() < mCapacity; });
            }

            mBuffer.push_back(std::move(message));

            if (mBuffer.size() == 1) {
                mCvRead.notify_one();
            }
        }


        /* *
         * Write a message to the buffer. Blocks for a duration
         * if the buffer is full.
         * @param message The message to be written
         * @param timeout The timeout to wait before unblocking
         * @return whether the write was successful or not
         */
        bool write(T message, duration timeout) {
            std::unique_lock<std::mutex> lock(mMutex);

            // Calling this function means you want
            // to restore the buffer into a usable state
            if (mKill) mKill = false;

            if (mBuffer.size() == mCapacity) {
                auto spaceAvailable = mCvWrite.wait_for(lock, timeout, [this]() { return mBuffer.size() < mCapacity; });

                if(!spaceAvailable) return false;
            }

            mBuffer.push_back(std::move(message));

            if (mBuffer.size() == 1) {
                mCvRead.notify_one();
            }

            return true;
        }

        /* *
         * Read a message. Blocks temporarily if the buffer is empty.
         * If unblock() is called, this read operation will unblock
         * @param timeout The timeout to block for if the buffer is empty
         * @returns an optional value T.
         */
        std::optional<T> read(duration timeout){
            std::unique_lock<std::mutex> lock(mMutex);

            if (mBuffer.size() == 0) {
                auto dataAvailable = mCvRead.wait_for(lock, timeout, [this](){ return mBuffer.size() > 0; });

                if (!dataAvailable) {
                    return {};
                }
            }

            auto element = mBuffer.front();
            mBuffer.pop_front();

            if (mBuffer.size() == mCapacity - 1) {
                mCvWrite.notify_one();
            }

            if (mBuffer.empty()) {
                mCvEmpty.notify_all();
            }

            return element;
        }

        /* *
         * @return the number of elements in the buffer
         */
        size_t size() const {
            std::unique_lock<std::mutex> lock(mMutex);
            return mBuffer.size();
        }

        /* *
         * @return whether the buffer is empty.
         */
        bool empty() const {
            std::unique_lock<std::mutex> lock(mMutex);
            return mBuffer.empty();
        }

        /**
         * @return whether the buffer is full.
         */
        bool full() const {
            std::unique_lock<std::mutex> lock(mMutex);
            return mBuffer.full();
        }

        /*
         * Wait until the buffer is empty, i.e. until drained
         * */
        void waitUntilEmpty () {
            std::unique_lock<std::mutex> lock(mMutex);
            if (mBuffer.empty()) return; // if already empty, just return
            mCvEmpty.wait(lock, [this]() { return mBuffer.empty(); });
        }

        /**
         * Unblock any blocking read operation. This
         * does not include timeout read.
         */
        void unblock() {
            std::unique_lock<std::mutex> lock(mMutex);
            mKill = true;
            mCvRead.notify_all();
        }

        messagebuffer(messagebuffer<T>&& mb) {
            std::unique_lock<std::mutex> lock(mb.mMutex);
            mBuffer = std::move(mb.mBuffer);
            mCapacity = mb.mCapacity;
            mKill = mb.mKill;
        }

        messagebuffer(const messagebuffer<T>& mb) {
            std::unique_lock<std::mutex> lock(mb.mMutex);
            mBuffer = mb.mBuffer;
            mCapacity = mb.mCapacity;
            mKill = mb.mKill;
        }

    private:

        bool mKill{false};
        const size_t mCapacity;
        mutable std::mutex mMutex;
        std::condition_variable mCvRead;
        std::condition_variable mCvWrite;
        std::condition_variable mCvEmpty;
        boost::circular_buffer<T> mBuffer;
    };
}



