/* *
 * threadpool; a thread that executes tasks posted in order.
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

#include "workerthread.h"
#include "messagebuffer.h"

namespace workshop {

    using namespace std::chrono_literals;

    template<typename T>
    using up = std::unique_ptr<T>;

    template <typename Task, typename... Args>
    class threadpool {
    public:

        /**
         * Construct a threadpool
         * @param taskCapacity the number of tasks the thread can hold
         * before blocking
         * @param numThreads the number of threads in this thread pool
         */
        threadpool(size_t taskCapacity, unsigned int numThreads)
        {
            using workerthread = workerthread<Task, Args...>;
            auto nThreads = std::min(numThreads, std::thread::hardware_concurrency());
            auto tasksPerThread = taskCapacity / nThreads;
            //mThreads = std::vector<workerthread>(nThreads, std::make_unique<workerthread>(tasksPerThread));
            mThreads.reserve(nThreads);
            for(auto i = 0; i < nThreads; i++){
                mThreads.push_back(std::make_unique<workerthread>(tasksPerThread));
            }
        }

        threadpool(size_t taskCapacity)
        : threadpool(taskCapacity, std::thread::hardware_concurrency()){}

        /**
         * Sync with the threadpool. Waits until all tasks are executed.
         */
        void sync() noexcept {
            for(auto& worker: mThreads) {
                worker->sync();
            }
        }

        /**
         * Post a task to be executed asynchronously
         * @param task a callable with proper argument set
         * @param args the arguments to call task with.
         */
        void postTask(Task&& task, Args&&... args) {
            for(size_t i = 0; i < mThreads.size() && mThreads[mRoundRobinIdx]->full(); i++) {
                mRoundRobinIdx = (mRoundRobinIdx + 1) % mThreads.size();
            }
            mThreads[mRoundRobinIdx]->postTask(std::forward<Task>(task), std::forward<Args>(args)...);
            mRoundRobinIdx = (mRoundRobinIdx + 1) % mThreads.size();
        }

        /**
         * Check whether the thread is in a joinable state
         * @return whether the thread is joinable or not.
         */
        bool joinable() const {
            return !mIsJoined;
        }

        /**
         * Join all the workerthreads
         */
        void join() noexcept {
            if (!mIsJoined) {
                for(auto& worker: mThreads) {
                    if (worker->joinable()) {
                        worker->join();
                    }
                }
                mIsJoined = true;
            }
        }

        ~threadpool() {
            join();
        }

    private:
        size_t mRoundRobinIdx = 0;
        bool mIsJoined {false};
        std::vector<up<workerthread<Task, Args...>>> mThreads;
    };
};
