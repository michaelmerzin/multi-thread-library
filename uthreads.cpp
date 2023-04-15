#include "uthreads.h"
#include "thread.h"
#include "Scheduler.h"
#include <queue>
#include <hash_map>
#include <unordered_map>


Scheduler *scheduler;
std::unordered_map<int, Thread *> threads;
std::unordered_map<int, Thread *> blocked_threads;


int uthread_init(int quantum_usecs) {
    if (quantum_usecs <= 0) {
        return -1;
    }


    Thread *main_thread = new Thread(RUNNING);
    threads[0] = main_thread;


    scheduler = new Scheduler(quantum_usecs, main_thread);
    return 0;
}


int uthread_spawn(thread_entry_point entry_point) {
    Thread *new_thread;
    try {
        new_thread = new Thread(READY, entry_point);
    }
    catch (no_available_tids_exception e) {
        return -1;
    }
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
    if (threads.find(tid) == threads.end())
        return -1;
    Thread *thread = threads[tid];

    threads.erase(tid);


    if (tid == uthread_get_tid()) {
        thread->set_state(TERMINATED);
        scheduler->reset_timer();
    } else {
        Scheduler::remove(thread->get_id());
        delete thread;
    }
    return 0;
}


int uthread_block(int tid) {
    if (tid == 0 || threads[tid] == nullptr) {
        return -1;
    }

    Thread *thread = threads[tid];
    thread->set_state(BLOCKED);
    Scheduler::block_thread(thread);
    if (tid == uthread_get_tid()) {
        scheduler->reset_timer();
    }
    return 0;
}


int uthread_resume(int tid) {
    if (threads[tid] == nullptr)
        return -1;
    if (tid == uthread_get_tid())
        return 0;
    Thread *thread = threads[tid];
    thread->set_state(READY);
    Scheduler::resume_thread(thread);
    return 0;
}

int uthread_sleep(int num_quantums) {
    int id = uthread_get_tid();
    Thread *thread = threads[id];
    if (thread->is_sleeping() || id == 0)
        return -1;

    thread->set_sleep_quantum(num_quantums + 1);
    Scheduler::sleep_thread(thread);

    scheduler->reset_timer();


    return 0;
}


int uthread_get_tid() {
    return scheduler->running_thread()->get_id();
}

int uthread_get_total_quantums() {

    return Scheduler::get_total_quantums();
}


int uthread_get_quantums(int tid) {
    if (threads[tid] == nullptr) {
        return -1;
    }
    return threads[tid]->get_quantum();

}

