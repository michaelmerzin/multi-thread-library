#ifndef USER_THREADS_SCHEDULER_H
#define USER_THREADS_SCHEDULER_H

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include "thread.h"
#include "uthreads.h"



class Scheduler {
private:
    int quantum_usecs;
    struct sigaction sa = {0} ;
    struct itimerval timer;

public:
    static std::queue<Thread*> ready_threads;
    static bool add(Thread* thread);
    static bool remove(int tid);

    explicit Scheduler(int quantum_usecs, Thread* main_thread);
    void start_timer();
    static Thread* running_thread() { return Scheduler::ready_threads.front();}
    ~Scheduler();
};


#endif //USER_THREADS_SCHEDULER_H
