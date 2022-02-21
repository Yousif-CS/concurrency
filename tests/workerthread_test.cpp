
#include <iostream>
#include <functional>
#include <gtest/gtest.h>
#include "concurrency/workerthread.h"



TEST(workerthread, executeOneTask) {
    
    workshop::workerthread<std::function<void()>>  wt{1};

    wt.postTask([](){ 
        std::cout << "Executing assertion task" << '\n';
        ASSERT_TRUE(true);
    });

    if(wt.joinable()) wt.join();
}
