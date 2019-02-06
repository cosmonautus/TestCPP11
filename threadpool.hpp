#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

template<typename T> struct AData
{
    T data;
    volatile bool ready;
    AData() : ready(false) {};
};

class ThreadPool
{
private:

    using fn_type = std::function<void()>;

    class Worker
    {
    private:
        bool enabled;
        std::condition_variable cv;
        std::queue<fn_type> fqueue;
        std::mutex mutex;
        std::thread thread;

        void thread_fn()
        {
            while (enabled)
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [&]() { return !fqueue.empty() || !enabled; });
                while (!fqueue.empty())
                {
                    fn_type fn = fqueue.front();
                    lock.unlock();
                    fn();
                    lock.lock();
                    fqueue.pop();
                }
            }
        }
    public:
        Worker() : enabled(true), fqueue(), thread(&Worker::thread_fn, this)
        {
        }
        ~Worker()
        {
            enabled = false;
            cv.notify_one();
            thread.join();
        }
        void appendFn(fn_type fn)
        {
            std::unique_lock<std::mutex> lock(mutex);
            fqueue.push(fn);
            cv.notify_one();
        }
        size_t getTaskCount()
        {
            std::unique_lock<std::mutex> lock(mutex);
            return fqueue.size();
        }
        bool isEmpty()
        {
            std::unique_lock<std::mutex> lock(mutex);
            return fqueue.empty();
        }
    };

    using worker_ptr = std::shared_ptr<Worker>;

    std::vector<worker_ptr> _workers;

    worker_ptr getFreeWorker()
    {
        worker_ptr pWorker;
        size_t minTasks = UINT32_MAX;
        for (auto &it : _workers)
            if (it->isEmpty())
            {
                return it;
            }
            else
                if (minTasks > it->getTaskCount())
                {
                    minTasks = it->getTaskCount();
                    pWorker = it;
                }
        return pWorker;
    }

public:
    ThreadPool(size_t threads = 1)
    {
        if (threads == 0)
            threads = 1;
        for (size_t i = 0; i < threads; i++)
        {
            //worker_ptr pWorker(new Worker);
            //_workers.push_back(pWorker);
            _workers.push_back(std::make_shared<Worker>());
        }
    }
    ~ThreadPool()
    {
    }
    template<typename Fn, typename... ARGS>
    void runAsync(Fn _fn, ARGS... _args)
    {
        getFreeWorker()->appendFn(std::bind(_fn, _args...));
    }
    template<typename Tret, typename Fn, typename... ARGS>
    std::shared_ptr<AData<Tret>> runAsync(Fn _fn, ARGS... _args)
    {
        std::function<Tret()> rfn = std::bind(_fn, _args...);
        std::shared_ptr<AData<Tret>> pData = std::make_shared<AData<Tret>>();
        getFreeWorker()->appendFn(
            [=]()
            {
                pData->data = rfn();
                pData->ready = true;
            }
        );
        return pData;
    }
};

#endif // THREADPOOL_HPP
