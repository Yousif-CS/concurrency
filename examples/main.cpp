#include <iostream>
#include <functional>
#include <thread>
#include <mutex>

#include "concurrency/threadpool.h"


int main() {

    workshop::threadpool<std::function<void(int)>, int> pool(10, 5);
    std::mutex mtx;
    auto task = [&mtx](int idx) {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Hello World: " << idx << " from: " << std::this_thread::get_id() << '\n';
    };

    for(int i = 0; i < 10; i++){
        pool.postTask(task, int{i});
    }
    pool.sync();
    return 0;
}
