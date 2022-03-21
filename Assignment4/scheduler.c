/**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>

#include "simulator.h"
#include "scheduler.h"
#include "queue.h"

#define MAX_THREADS 4650

typedef struct _thread_list {
  unsigned int arrival_time;
  unsigned int start_time;
  unsigned int waiting_time;
  unsigned int exit_time;
} thread_list;

thread_t *ready_queue;
thread_t *current_thread;
thread_list *t_list;
enum algorithm current_algorithm;
int thread_count;
int quant;
int current_quant;

//dispatches threads from the ready queue
static void dispatch_thread(void){
  thread_t *t = queue_dequeue(ready_queue);  
  if (t != NULL){
    current_thread = t;
    t_list[t->tid-1].waiting_time += sim_time() - t_list[t->tid-1].start_time + 1;
    sim_dispatch(t);
    current_quant = 0;
  }
}

//compares two threads priorities
static int thread_comparator(void *a, void *b){
  return ((thread_t *)a)->priority - ((thread_t *)b)->priority;
}

//adds to the ready queue based on whichever algorithm is running
static void add_to_ready_queue(thread_t *t){
  switch (current_algorithm){
    case FIRST_COME_FIRST_SERVED:
      if (current_thread == NULL){
        current_thread = t;
        sim_dispatch(t);
      }
      else {
        queue_enqueue(ready_queue, t);
      }
      break;
    case NON_PREEMPTIVE_PRIORITY:
      queue_enqueue(ready_queue, t);
      queue_sort(ready_queue, &thread_comparator);
      t_list[t->tid-1].waiting_time--;
      break;
    case PREEMPTIVE_PRIORITY:
      queue_enqueue(ready_queue, t);
      queue_sort(ready_queue, &thread_comparator);
      t_list[t->tid-1].waiting_time--;
      break;
    case ROUND_ROBIN:
      queue_enqueue(ready_queue, t);
      t_list[t->tid-1].waiting_time--;
      break;
    default:
      break;
  }
}

/**
 * Initalise a scheduler implemeting the requested ALGORITHM. QUANTUM is only
 * meaningful for ROUND_ROBIN.
 */
void scheduler(enum algorithm algorithm, unsigned int quantum) {
  t_list = malloc(sizeof(thread_list) * MAX_THREADS);
  ready_queue = queue_create();
  current_thread = NULL;
  current_algorithm = algorithm;
  thread_count = 0;
  quant = quantum;
  current_quant = 0;
}

/**
 * Thread T is ready to be scheduled for the first time.
 */
void sys_exec(thread_t *t) {
  thread_count++;
  t_list[t->tid-1].arrival_time = sim_time();
  t_list[t->tid-1].start_time = sim_time();
  t_list[t->tid-1].waiting_time = 0;
  t_list[t->tid-1].exit_time = 0;
  add_to_ready_queue(t);
}

/**
 * Programmable clock interrupt handler. Call sim_time() to find out
 * what tick this is. Called after all calls to sys_exec() for this
 * tick have been made.
 */
void tick() { 
  switch (current_algorithm){
    case FIRST_COME_FIRST_SERVED:
      break;
    case NON_PREEMPTIVE_PRIORITY:
      if (current_thread == NULL){
        dispatch_thread();
      }
      break;
    case PREEMPTIVE_PRIORITY:
      if (current_thread == NULL){
        dispatch_thread();
      }
      else {
        thread_t *head = queue_head(ready_queue);
        if (head != NULL && head->priority < current_thread->priority){
          queue_enqueue(ready_queue, current_thread);
          queue_sort(ready_queue, &thread_comparator);
          t_list[current_thread->tid-1].start_time = sim_time() + 1;
          dispatch_thread();
        }
      }
      break;
   case ROUND_ROBIN:
      ;
      thread_t *rrhead = queue_head(ready_queue);
      //friend helped me do this part, was stuck for awhile, probably very similar to his 
      if (current_thread != NULL){
        current_quant++;
      }
      else {
        dispatch_thread();
      }
      if (rrhead == NULL){
        current_quant %= quant;
      }
      else if (current_quant == quant){
        queue_enqueue(ready_queue, current_thread);
        t_list[current_thread->tid-1].start_time = sim_time() + 1;
        dispatch_thread();
      }
      break;
    default:
      break;
  }
}

/**
 * Thread T has completed execution and should never again be scheduled.
 */
void sys_exit(thread_t *t) {
  current_thread = NULL;
  t_list[t->tid-1].exit_time = sim_time();
  if (current_algorithm == FIRST_COME_FIRST_SERVED){
    dispatch_thread();
  }
}


/**
 * Thread T has requested a read operation and is now in an I/O wait queue.
 * When the read operation starts, io_starting(T) will be called, when the
 * read operation completes, io_complete(T) will be called.
 */
void sys_read(thread_t *t) {
  current_thread = NULL;
  t_list[t->tid-1].start_time = sim_time() + 1;  
  if (current_algorithm == FIRST_COME_FIRST_SERVED){
    dispatch_thread();
  }
}

/**
 * Thread T has requested a write operation and is now in an I/O wait queue.
 * When the write operation starts, io_starting(T) will be called, when the
 * write operation completes, io_complete(T) will be called.
 */
void sys_write(thread_t *t) {
  current_thread = NULL;
  t_list[t->tid-1].start_time = sim_time() + 1;
  if (current_algorithm == FIRST_COME_FIRST_SERVED){
    dispatch_thread();
  }
}

/**
 * An I/O operation requested by T has completed; T is now ready to be 
 * scheduled again.
 */
void io_complete(thread_t *t) {
  t_list[t->tid-1].start_time = sim_time() + 1;
  add_to_ready_queue(t);
}

/**
 * An I/O operation requested by T is starting; T will not be ready for
 * scheduling until the operation completes.
 */
void io_starting(thread_t *t){
  t_list[t->tid-1].waiting_time += sim_time() - t_list[t->tid-1].start_time;
}

/**
 * Return dynamically allocated stats for the scheduler simulation, see 
 * scheduler.h for details. Memory allocated by your code will be free'd
 * by the similulator. Do NOT return a pointer to a stack variable.
 */
stats_t *stats() {
  int mean_waiting_time = 0;
  int mean_turnaround_time = 0;
  stats_t *stats = malloc(sizeof(stats_t));
  stats->thread_count = thread_count;
  stats->tstats = malloc(sizeof(stats_t)*stats->thread_count);
  int temp = 0;
  for (int i = 0; i < thread_count; i++){
    temp = t_list[i].exit_time - t_list[i].arrival_time + 1;
    stats->tstats[i].tid = i + 1;
    stats->tstats[i].waiting_time = t_list[i].waiting_time;
    stats->tstats[i].turnaround_time = temp;
    mean_waiting_time += t_list[i].waiting_time;
    mean_turnaround_time += temp;
  }
  stats->waiting_time = mean_waiting_time / thread_count;
  stats->turnaround_time = mean_turnaround_time / thread_count;
  return stats; 
}
