/************************************************************************
 *
 * CSE130 Assignment 5
 *
 * Copyright (C) 2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 * 
 ************************************************************************/

#include "simulator.h"
#include "queue.h"
#include "stdio.h"


void *ready_queue;
enum algorithm current_algorithm;
unsigned int current_frames;
unsigned int current_page;

typedef struct {
  int a;
} inner_t;

typedef struct {
  inner_t *inner;
  int x;
} outer_t;

outer_t outer;
inner_t inner;

static bool inner_equalitor(void *outer1, void *inner1) {
   return (((outer_t*)outer1)->inner)->a == ((inner_t*)inner1)->a;
}

/**
 * Initialise your ALGORITHM pager with FRAMES frames (physical pages).
 */
void pager_init(enum algorithm algorithm, unsigned int frames) {
  
  ready_queue = queue_create();
  current_algorithm = algorithm;
  current_frames = frames;
  current_page = 0;
  outer.x = 0;
  inner.a = 0;
}

/**
 * Free any dynamically allocated resources.
 */
void pager_destroy() {
  queue_destroy(ready_queue);
}

/**
 * A request has been made for virtual page PAGE. If your pager does
 * not currently have PAGE in physical memory, request it from the 
 * simulator.
 */
void pager_request(unsigned int page) {
  switch(current_algorithm){
    case FIRST_IN_FIRST_OUT:
      inner.a = page;
      outer.inner = &inner;
      outer.x = page;
      if(queue_size(ready_queue) == current_frames){
        if(queue_find(ready_queue, inner_equalitor, &inner) == NULL){
          queue_dequeue(ready_queue);
          queue_enqueue(ready_queue, &outer);
          sim_get_page(page);
        }
      }
      else {
        if(queue_find(ready_queue, inner_equalitor, &inner) == NULL){//this function dont work and not sure why, pretty sure its because of the pointer
          queue_enqueue(ready_queue, &outer);
          sim_get_page(page);
        }
      }
      break;
    case LEAST_RECENTLY_USED:
      break;
    case LEAST_FREQUENTLY_USED: // Infinite history
      break;
    case SECOND_CHANCE:
      break;
    case CIRCULAR_QUEUE:
      break;
  }
}  
