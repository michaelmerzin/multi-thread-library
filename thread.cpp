#include "thread.h"

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
            : "=g" (ret)
            : "0" (addr));
    return ret;
}
#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5


/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}
#endif

int arr[MAX_THREAD_NUM+1]={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                           11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                           21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                           31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                           41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                           51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
                           61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
                           71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
                           81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
                           91, 92, 93, 94, 95, 96, 97, 98, 99, 100};
int N = sizeof(arr) / sizeof(arr[0]);
priority_queue <int, vector<int>, greater<int>> Thread::heap_thread_ids(arr, arr + N);







Thread::Thread(State state, thread_entry_point entry_point) {
    this->quantum = 0;
    this->thread_id = Thread::heap_thread_ids.top();
    Thread::heap_thread_ids.pop();
    this->state = state;
    this->stack = new char[STACK_SIZE];
    this->setup_thread(this->thread_id, this->stack, entry_point);
}

Thread::Thread(State state) {
    this->quantum = 0;
    this->thread_id = Thread::heap_thread_ids.top();
    Thread::heap_thread_ids.pop();
    this->state = state;
    sigsetjmp(env, 1);
}


void Thread::set_state(State state) {
    if (state == RUNNING)
        quantum += 1;
    this->state=state;
}

Thread::~Thread() {
    delete[] stack;
    Thread::heap_thread_ids.push(this->thread_id);
}


void Thread::setup_thread(int tid, char *stack, thread_entry_point entry_point) {
    // initializes env[tid] to use the right stack, and to run from the function 'entry_point', when we'll use
    // siglongjmp to jump into the thread.

    address_t sp = (address_t) stack + STACK_SIZE - sizeof(address_t);
    address_t pc = (address_t) entry_point;
    sigsetjmp(env, 1);
    (env->__jmpbuf)[JB_SP] = translate_address(sp);
    (env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&env->__saved_mask);
}

