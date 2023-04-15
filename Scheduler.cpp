#include "Scheduler.h"

std::queue<Thread *> Scheduler::ready_threads;
std::unordered_map<int, Thread *> Scheduler::sleeping_threads;
std::unordered_map<int, Thread *> Scheduler::block_threads;
int Scheduler::total_quantums = 1;

void handle_sleeping_threads();

Scheduler::Scheduler(int quantum_usecs, Thread *main_thread) {
    this->quantum_usecs = quantum_usecs;
    Scheduler::add(main_thread);
    this->start_timer();
}

bool Scheduler::add(Thread *thread) {
    if (Scheduler::ready_threads.size() > MAX_THREAD_NUM) { return false; }
    Scheduler::ready_threads.push(thread);
    return true;
}


Scheduler::~Scheduler() {
    for (int i = 0; i < Scheduler::ready_threads.size(); i++) {
        delete Scheduler::ready_threads.front();
        Scheduler::ready_threads.pop();

    }

}

bool Scheduler::remove(int tid) {
    std::queue<Thread *> ref;
    int s = Scheduler::ready_threads.size();
    int cnt = 0;

    // Finding the value to be removed
    while (!Scheduler::ready_threads.empty() and Scheduler::ready_threads.front()->get_id() != tid) {
        ref.push(Scheduler::ready_threads.front());
        Scheduler::ready_threads.pop();
        cnt++;
    }

    // If element is not found
    if (Scheduler::ready_threads.empty()) {
        while (!ref.empty()) {

            // Pushing all the elements back into q
            Scheduler::ready_threads.push(ref.front());
            ref.pop();
        }
        return false;
    }

        // If element is found
    else {
        Scheduler::ready_threads.pop();
        while (!ref.empty()) {

            // Pushing all the elements back into q
            Scheduler::ready_threads.push(ref.front());
            ref.pop();
        }
        int k = s - cnt - 1;
        while (k--) {

            // Pushing elements from front of q to its back
            Thread *p = Scheduler::ready_threads.front();
            Scheduler::ready_threads.pop();
            Scheduler::ready_threads.push(p);
        }
    }
    return true;
}

void handle_sleeping_threads() {
    std::vector<int> erased;
    for (auto &it: Scheduler::sleeping_threads) {
        it.second->decrease_sleep_quantum();
        if (it.second->is_sleeping())
            continue;
        uthread_resume(it.first);
        erased.push_back(it.first);


    }
    for (int id: erased) {
        Scheduler::sleeping_threads.erase(id);
    }
}

void timer_handler(int sig) {
    Thread *finished_thread = Scheduler::ready_threads.front();
    int return_value = sigsetjmp(*finished_thread->get_env(), 1);
    if (return_value != 0)
        return;
    handle_sleeping_threads();
    Scheduler::total_quantums += 1;

    if (Scheduler::sleeping_threads.find(finished_thread->get_id()) == Scheduler::sleeping_threads.end() &&
        Scheduler::block_threads.find(finished_thread->get_id()) == Scheduler::block_threads.end()
        && finished_thread->get_state() != TERMINATED) {
        Scheduler::ready_threads.pop();
        finished_thread->set_state(READY);
        Scheduler::ready_threads.push(finished_thread);
    } else {
        finished_thread->set_state(BLOCKED);
        Scheduler::block_thread(finished_thread);
        Scheduler::remove(finished_thread->get_id());
    }
    if (finished_thread->get_state() == TERMINATED) {
        Scheduler::remove(finished_thread->get_id());
        delete finished_thread;
    }

    Scheduler::ready_threads.front()->set_state(RUNNING);
    siglongjmp(*Scheduler::ready_threads.front()->get_env(), Scheduler::ready_threads.front()->get_id());
}


void Scheduler::start_timer() {
    this->sa = {0};

    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
        printf("sigaction error.");
    }

    // Configure the timer to expire after 1 sec... */


    timer.it_value.tv_sec = (quantum_usecs / SECOND);// first time interval, seconds part
    timer.it_value.tv_usec = (quantum_usecs % SECOND);// first time interval, microseconds part


    // configure the timer to expire every 3 sec after that.
    timer.it_interval.tv_sec = (quantum_usecs / SECOND);
    timer.it_interval.tv_usec = (quantum_usecs % SECOND);

    // Start a virtual timer. It counts down whenever this process is executing.

    if (setitimer(ITIMER_VIRTUAL, &this->timer, NULL)) {
        printf("setitimer error.");
    }


}