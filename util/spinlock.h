//
// Created by 彭琳峰 on 2020-10-26.
//

#ifndef MYDB_SPINLOCK_H
#define MYDB_SPINLOCK_H


#include <atomic>

class SpinLock {
public:
    void Lock() {
        while (lck.test_and_set(std::memory_order_acquire)) {}
    }

    void Unlock() {
        lck.clear(std::memory_order_release);
    }

private:
    std::atomic_flag lck = ATOMIC_FLAG_INIT;
};


#endif //MYDB_SPINLOCK_H
