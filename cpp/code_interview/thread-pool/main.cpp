#include "gtest/gtest.h"
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <exception>
#include <atomic>

using namespace std;

class StopException: runtime_error {
public:
    StopException() = default;
    explicit StopException(const string& arg): runtime_error(arg) {}
};

template<typename T>
class BlockQueue {
    deque<T> m_queue{};
    mutex m_lock{};
    condition_variable m_cv;
    bool m_stoped = false;

public:

    template<typename Arg>
    void put(Arg&& arg) {
        {
            unique_lock<mutex> lock_guard(m_lock);
            m_queue.emplace_back(forward<Arg>(arg));
        }
        m_cv.notify_one();
    }

    T get() {
        unique_lock<mutex> lock_guard(m_lock);
        m_cv.wait(lock_guard, [&](){return (!m_queue.empty() || m_stoped);});
        if (m_stoped && m_queue.empty()) {
            throw StopException("stopped");
        }
        auto ret = m_queue.front();
        m_queue.pop_front();
        return ret;
    }

    void stop() {
        {
            unique_lock<mutex> lock_guard(m_lock);
            m_stoped = true;
        }
        m_cv.notify_all();
    }
};

class ThreadPool {
public:
    using Runable = function<void ()>;

private:
    BlockQueue<Runable> m_queue{};
    vector<thread> m_runners;

    static void run_task (ThreadPool *pool) {
        try {
            while (true) {
                auto call = pool->m_queue.get();
                call();
            }
        } catch (StopException &e) {
        }
    }

public:
    explicit ThreadPool(int tasks): m_runners() {
        while (tasks--) {
            m_runners.emplace_back(thread(run_task, this));
        }
    }

    void stop() {
        m_queue.stop();
    }

    void join() {
        for (auto &th: m_runners) {
            th.join();
        }
    }

    template<typename Arg>
    void submit(Arg&& arg) {
        m_queue.put(forward<Arg>(arg));
    }
};

TEST(test, thread_pool) {
    ThreadPool pool(5);
    atomic<int> cnt(0);
    for (auto i = 0; i < 100; ++i) {
        pool.submit([&cnt](){++cnt;});
    }
    pool.stop();
    pool.join();
    ASSERT_EQ(int(cnt), 100);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
