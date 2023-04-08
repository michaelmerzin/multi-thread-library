#include "Scheduler.h"

std::queue<Thread*> Scheduler::ready_threads;
Scheduler::Scheduler(int quantum_usecs, Thread* main_thread) {
    this->quantum_usecs = quantum_usecs;
    Scheduler::add(main_thread);
    this->start_timer();
}

bool Scheduler::add(Thread* thread) {
    if (Scheduler::ready_threads.size() > MAX_THREAD_NUM) {return false;}
    Scheduler::ready_threads.push(thread);
    return true;
}


Scheduler::~Scheduler()
{
    for (int i = 0; i < Scheduler::ready_threads.size(); i++) {
        delete Scheduler::ready_threads.front();

    }
    delete this;
}

bool Scheduler::remove(int tid) {
    std::queue<Thread*> ref;
    int s = Scheduler::ready_threads.size();
    int cnt = 0;

    // Finding the value to be removed
    while (Scheduler::ready_threads.front()->get_id() != tid and !Scheduler::ready_threads.empty()) {
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
            Thread* p = Scheduler::ready_threads.front();
            Scheduler::ready_threads.pop();
            Scheduler::ready_threads.push(p);
        }
    }
    return true;
}




void timer_handler(int sig)
{
    Thread* finished_thread = Scheduler::ready_threads.front();
    int return_value = sigsetjmp(*finished_thread->get_env(), 1);

    if(return_value == 0 ) {
        Scheduler::ready_threads.pop();
        finished_thread->set_state(READY);
        Scheduler::ready_threads.push(finished_thread);
        Scheduler::ready_threads.front()->set_state(RUNNING);
        siglongjmp(*Scheduler::ready_threads.front()->get_env(), Scheduler::ready_threads.front()->get_id());
    }
}


void Scheduler::start_timer(){
    this->sa = {0};

    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0)
    {
        printf("sigaction error.");
    }
    // Configure the timer to expire after 1 sec... */
    this->timer.it_value.tv_sec = 0;        // first time interval, seconds part
    this->timer.it_value.tv_usec = this->quantum_usecs;        // first time interval, microseconds part

    // configure the timer to expire every 3 sec after that.
    this->timer.it_interval.tv_sec = 0;    // following time intervals, seconds part
    this->timer.it_interval.tv_usec = this->quantum_usecs;    // following time intervals, microseconds part

    // Start a virtual timer. It counts down whenever this process is executing.
    if (setitimer(ITIMER_VIRTUAL, &this->timer, NULL))
    {
        printf("setitimer error.");
    }
}