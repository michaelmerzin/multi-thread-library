
#ifndef USER_THREADS_THREAD_H
#define USER_THREADS_THREAD_H




#include "uthreads.h"
#include <queue>
#include <algorithm>
#include <signal.h>
#include <setjmp.h>


using namespace std;
enum State {RUNNING, READY, BLOCKED};


class Thread {
private:
    void setup_thread(int tid, char *stack, thread_entry_point entry_point);

    int thread_id;
    State state;
    char* stack;
    sigjmp_buf env;
    int quantum;


public:
    static priority_queue <int, vector<int>, greater<int>> heap_thread_ids;
    Thread(State state);
    Thread(State state, thread_entry_point entry_point);
    ~Thread();
    void set_state(State state);
    int get_id() const  {return this->thread_id;}
    int get_quantum() const  {return this->quantum;}
    char* get_stack() const {return this->stack;}
    sigjmp_buf *get_env()  {return & (this->env);}


};


#endif //USER_THREADS_THREAD_H
