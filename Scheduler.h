#ifndef USER_THREADS_SCHEDULER_H
#define USER_THREADS_SCHEDULER_H

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unordered_map>
#include "thread.h"
#include "uthreads.h"

#define SECOND 1000000

class Scheduler {
private:
    int quantum_usecs;
    struct sigaction sa = {0};
    struct itimerval timer;

public:
    static std::queue<Thread *> ready_threads;
    static std::unordered_map<int, Thread *> sleeping_threads;
    static std::unordered_map<int, Thread *> block_threads;
    static int total_quantums;

    static void block_thread(Thread *thread) { Scheduler::block_threads[thread->get_id()] = thread; }

    static void resume_thread(Thread *thread) {
        if (Scheduler::block_threads.find(thread->get_id()) == Scheduler::block_threads.end())
            return;
        Scheduler::block_threads.erase(thread->get_id());
        Scheduler::add(thread);
    }

    static bool add(Thread *thread);

    static bool remove(int tid);

    static int get_total_quantums() { return total_quantums; }

    explicit Scheduler(int quantum_usecs, Thread *main_thread);

    void start_timer();

    static void sleep_thread(Thread *thread) { Scheduler::sleeping_threads[thread->get_id()] = thread; }

    static Thread *running_thread() { return Scheduler::ready_threads.front(); }

    void reset_timer() {

        start_timer();
        raise(SIGVTALRM);
    }

    ~Scheduler();
};


#endif //USER_THREADS_SCHEDULER_H
