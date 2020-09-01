/**
 * a reader writer pattern cpp implementation
 */

#include "gtest/gtest.h"
#include <mutex>
#include <map>
#include <thread>
#include <iostream>
#include <chrono>

using namespace std;

class RWLock {
public:
    virtual void rlock() = 0;
    virtual void runlock() = 0;
    virtual void wlock() = 0;
    virtual void wunlock() = 0;
};

/**
 * a reader preferred lock
 */
class RWLock1: public RWLock{
public:
    void rlock() override {
        unique_lock<mutex> guard(m_reader_lock);
        ++m_count;
        if (m_count == 1) {
            m_res_lock.lock();
        }
    }

    void runlock() override {
        unique_lock<mutex> guard(m_reader_lock);
        --m_count;
        if (m_count == 0) {
            m_res_lock.unlock();
        }
    }

    void wlock() override {
        m_res_lock.lock();
    }

    void wunlock() override {
        m_res_lock.unlock();
    }

private:
    mutex m_res_lock; // resource lock
    mutex m_reader_lock; // reader count lock
    int m_count = 0; // reader count
};

/**
 * 读写均衡的锁, 依靠mutex自己的公平性来保证
 */
class RWLock2: public RWLock {
public:
    void rlock() override {
        lock_guard<mutex> ser_guard(m_ser_lock);
        lock_guard<mutex> read_guard(m_reader_lock);
        ++m_read_count;
        if (m_read_count == 1) {
            m_res_lock.lock();
        }
    }

    void runlock() override {
        lock_guard<mutex> read_guard(m_reader_lock);
        --m_read_count;
        if (m_read_count == 0) {
            m_res_lock.unlock();
        }
    }

    void wlock() override {
        lock_guard<mutex> ser_guard(m_ser_lock);
        m_res_lock.lock();
    }

    void wunlock() override {
        m_res_lock.unlock();
    }
private:
    mutex m_res_lock; // resource lock
    mutex m_ser_lock; // 服务锁，只有拿到服务锁才能被服务, 只有开始服务才释放锁
    mutex m_reader_lock; // reader count lock
    int m_read_count = 0; // 读计数
};

void run_rwlock_test(int r_count, int rw_ratio, RWLock &lock) {
    ASSERT_TRUE(rw_ratio > 0);
    vector<int> data;
    vector<thread> ths;
    for (int i = 0; i < r_count; ++i) {
        // add reader
        ths.emplace_back(thread([&, i=i]{
            cout << "reader [" << i << "] started" << endl;
            lock.rlock();
            cout << "reader [" << i << "] reading: " << data.size() << endl;
            this_thread::sleep_for(chrono::milliseconds(1));
            lock.runlock();
            cout << "reader [" << i << "] done" << endl;
        }));

        // add writer
        if (i % rw_ratio == 0) {
            ths.emplace_back(thread([&, i=i/rw_ratio]{
                cout << "writer [" << i << "] started" << endl;
                lock.wlock();
                data.push_back(i);
                cout << "writer [" << i << "] writing: " << data.size() << endl;
                this_thread::sleep_for(chrono::milliseconds(1));
                lock.wunlock();
                cout << "writer [" << i << "] done" << endl;
            }));
        }
    }

    for (auto &th: ths) {
        th.join();
    }

    ASSERT_EQ(data.size(), r_count / rw_ratio);
}

TEST(test, rwlock) {
    RWLock1 lock;
    run_rwlock_test(100, 5, lock);
    cout << "=========== lock2 =========" << endl;
    RWLock2 lock2;
    run_rwlock_test(100, 5, lock2);
}


int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}