/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "pthread.h"
#include "semaphore.h"
#include "cartman.h"

#define MAX_SEMS 6
#define MAX_JUNC 7

sem_t sems[MAX_SEMS];
int juncCheck[MAX_JUNC] = {0};
typedef struct cartData_t{
  int cart; 
  enum track track;
  enum junction junction;
} cartData;
cartData carts[MAX_SEMS];

void *thread_func(void *data){
  cartData *args = (cartData *) data;
  if(juncCheck[args->cart] == 1 || juncCheck[args->cart + 1] == 1){
    exit(0);
  }
  juncCheck[args->cart] = 1;
  juncCheck[args->cart + 1] = 1;
  sem_wait(&sems[args->cart]);
  reserve(args->cart, args->junction);
  sem_wait(&sems[args->cart + 1]);
  reserve(args->cart, args->junction + 1);
  cross(args->cart, args->track, args->junction);
  pthread_exit(NULL);
}

/*
 * Callback when CART on TRACK arrives at JUNCTION in preparation for
 * crossing a critical section of track.
 */
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
  pthread_t thread;
  carts[cart].cart = cart;
  carts[cart].track = track;
  carts[cart].junction = junction;
  pthread_create(&thread, NULL, thread_func, (void *)&carts[cart]);
//  sem_post(&sems[cart]); 
//  release(cart, junction);
//  sem_post(&sems[cart + 1]);
//  release(cart, junction + 1);
}

/*
 * Start the CART Manager for TRACKS tracks.
 *
 * Return is optional, i.e. entering an infinite loop is allowed, but not
 * recommended. Some implementations will do nothing, most will initialise
 * necessary concurrency primitives.
 */
void cartman(unsigned int tracks) 
{
  for (int i = 0; i < MAX_SEMS; i++){
    sem_init(&sems[i],0,1);
  }
}
