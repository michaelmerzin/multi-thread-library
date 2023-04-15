
#ifndef USER_THREADS_THREAD_H
#define USER_THREADS_THREAD_H


#include "uthreads.h"
#include <queue>
#include <algorithm>
#include <signal.h>
#include <setjmp.h>
#include "no_available_tids_exception.h"

using namespace std;
enum State {
    RUNNING, READY, BLOCKED, TERMINATED
};


class Thread {
private:
    void setup_thread(int tid, char *stack, thread_entry_point entry_point);

    int thread_id;
    State state;
    char *stack;
    sigjmp_buf env;
    int quantum;
    int sleep_quantum;


public:
    static priority_queue<int, vector<int>, greater<int>> available_tids;

    Thread(State state);

    Thread(State state, thread_entry_point entry_point);

    ~Thread();

    void set_state(State state);

    State get_state() const { return state; }

    int get_id() const { return this->thread_id; }

    int get_quantum() const { return this->quantum; }

    char *get_stack() const { return this->stack; }

    sigjmp_buf *get_env() { return &(this->env); }

    void decrease_sleep_quantum() { this->sleep_quantum = max(0, this->sleep_quantum - 1); }

    void set_sleep_quantum(int sleep_quantum) { this->sleep_quantum = sleep_quantum; }

    bool is_sleeping() const { return this->sleep_quantum > 0; }
};


#endif //USER_THREADS_THREAD_H
