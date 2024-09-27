#ifndef POOL_H
#define POOL_H

#include <queue>
#include <functional>
#include <condition_variable>
#include <vector>
#include <future>
#include <iostream>

// 线程池类
class Pool {
public:
    Pool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back(
                [this] {
                    for (;;) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queueMutex);
                            this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
            );
        }
    }
    
    ~Pool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
            worker.join();
    }
    
    template<class F>
    auto enqueue(F&& f) -> std::future<decltype(f())> {
        using return_type = decltype(f());
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // 不允许在停止状态下插入新任务
            if (stop)
                throw std::runtime_error("enqueue on stopped Pool");
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

private:
    std::vector<std::thread> workers;  
    std::queue<std::function<void()>> tasks; 
    std::mutex queueMutex;               
    std::condition_variable condition;    
    bool stop;                           
};

#endif // POOL_H
