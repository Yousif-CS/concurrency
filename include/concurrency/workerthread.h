/* *
 * workerthread; a thread that executes tasks posted in order.
 * Created by Yousif on Feb 13th 2022.
 * */

#pragma once

#include <thread>
#include <tuple>
#include <utility>
#include <atomic>
#include <chrono>
#include <vector>
#include <algorithm>

#include "messagebuffer.h"


namespace workshop {

    using namespace std::chrono_literals;

    template <typename Task, typename... Args>
    class workerthread {
    public:

        /**
         * Construct a workerthread
         * @param taskCapacity the number of tasks the thread can hold
         * before blocking
         */
        explicit workerthread(size_t taskCapacity)
        : mTaskQueue(taskCapacity){
            mThread = std::thread(&workerthread::process, this);
        }

        /**
         * Join the current thread pool. Joins all the threads in the
         * workerthread
         */
        void join() noexcept {
            if (!mIsJoined){
                // Writing an empty message means stop
                mTaskQueue.write({});
                mThread.join();
                mIsJoined = true;
            }
        }

        /* *
         * Block until all tasks are executed
         */
        void sync() {
            mTaskQueue.waitUntilEmpty();
        }

        /**
         * Post a task to be executed asynchronously
         * @param task a callable with proper argument set
         * @param args the arguments to call task with.
         */
        void postTask(Task&& task, Args&&... args) {
            auto f = std::function<void(Args...)>(std::forward<Task>(task));
            mTaskQueue.write(std::make_tuple(f, std::forward<Args>(args)...));
        }


        bool full() {
            return mTaskQueue.full();
        }

        /**
         * Check whether the thread is in a joinable state
         * @return whether the thread is joinable or not.
         */
        bool joinable() const noexcept {
            return !mIsJoined;
        }

        workerthread(workerthread&&) = default;

        ~workerthread() {
            mIsStopped = true;
            mTaskQueue.unblock();
            if (mThread.joinable()) {
                mThread.join();
            }
        }

    private:

        using Message = std::optional<std::tuple<Task, Args...>>;

        /* *
         * the main processing loop
         */
        void process() {
            mIsStopped = false;

            while(!mIsStopped) {
                auto message = mTaskQueue.read();
                if (mIsStopped) break; // forcibly stopped
                if (!message) break; // the read was unblocked
                if (!*message) break; // a stop message was sent
                auto execute = [](Task t, Args... args) {
                    t(args...);
                };
                std::apply(execute, **message);
            }
        }

        bool mIsJoined {false};
        std::atomic<bool> mIsStopped {true};
        std::thread mThread;
        messagebuffer<Message> mTaskQueue;
    };
};
