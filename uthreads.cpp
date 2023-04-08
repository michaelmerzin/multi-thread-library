#include "uthreads.h"
#include "thread.h"
#include "Scheduler.h"
#include <queue>
#include <hash_map>
#include <unordered_map>


Scheduler* scheduler;
std::unordered_map<int, Thread *> threads;
std::unordered_map<int, Thread *> blocked_threads;


int uthread_init(int quantum_usecs) {
    if (quantum_usecs <= 0) {
        return -1;
    }
    //init the main thread

//    threads = std::unordered_map<int, Thread*>();
//    blocked_threads = std::unordered_map<int, Thread*>();

    Thread *main_thread = new Thread(RUNNING);
    threads[0] = main_thread;


    Scheduler* scheduler = new Scheduler(quantum_usecs, main_thread);
    return 0;
}


int uthread_spawn(thread_entry_point entry_point) {
    Thread *new_thread = new Thread(READY, entry_point);
    if (!Scheduler::add(new_thread)) {
        return -1;
    }
    threads[new_thread->get_id()] = new_thread;
    return new_thread->get_id();
}


int uthread_terminate(int tid) {
    if (tid == 0) {
        delete scheduler;
        exit(0);
    }

    if (!Scheduler::remove(tid)) {
        return -1;
    }
    return 0;
}


int uthread_block(int tid) {
    if (tid == 0 || threads[tid] == nullptr) {
        return -1;
    }
    Thread *thread = threads[tid];
    thread->set_state(BLOCKED);
    Scheduler::remove(tid);
    blocked_threads[tid] = thread;
    return 0;
}


int uthread_resume(int tid) {
    if (threads[tid] == nullptr) {
        return -1;
    }
    Thread *thread = threads[tid];
    thread->set_state(READY);
    Scheduler::add(thread);
    blocked_threads.erase(tid);
    return 0;
}

int uthread_sleep(int num_quantums) {
    return 0;
}


int uthread_get_tid() {
    return scheduler->running_thread()->get_id();
}

int uthread_get_total_quantums() {
    int total_quantums = 0;
    for (auto it = threads.begin(); it != threads.end(); ++it) {
        total_quantums += uthread_get_quantums(it->second->get_id());
    }
    return total_quantums;
}


int uthread_get_quantums(int tid) {
    if (threads[tid] == nullptr) {
        return -1;
    }
    return threads[tid]->get_quantum();

}

