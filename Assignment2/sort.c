#include "merge.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define THREADS 4

typedef struct _targ_t {
  int left;
  int right;
  int inc;//so i no which thread im on
  int *arr;
  int prevLeft;//need prevleft to do the merge with both threads
} targ_t;

pthread_t threadList[THREADS];

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

void *thread_routine(void *arg) {
  targ_t *i = (targ_t *) arg; //lets me use tac[i]
  int middle = (i->prevLeft + i->right)/2;
  singleThreadedMergeSort(i->arr, i->left, i->right);
  if (i->inc % 2  == 1){//only trying to get threads  1 and 3
    pthread_join(threadList[i->inc - 1], NULL);//join threads 0 and 2
    merge(i->arr, i->prevLeft, middle, i->right);
  }
  pthread_exit(NULL);
}

/* 
 * This function stub needs to be completed
 */
void multiThreadedMergeSort(int arr[], int left, int right) 
{
  // Your code goes here
  targ_t tas[THREADS];
  int middle = (left + right) / 2;
  int temp = 0;
  int split4[] = {left, (left+middle)/2, (left+middle)/2 + 1, middle, middle + 1, (right+middle)/2, (right+middle)/2 + 1,right};//I couldn't figure out how to increment it in the for loop so I hardcoded in with 4 threads. Won't work for a different number of threads
  for (int i = 0; i < THREADS; i++){
    tas[i].left = split4[2 * i];
    tas[i].right = split4[2 * i + 1];
    tas[i].arr = arr;
    tas[i].inc = i;
    tas[i].prevLeft = temp;//temp to get previous threads left for merging
    temp = split4[2 * i];
    pthread_create(&threadList[i], NULL, thread_routine, (void*) &tas[i]);    
  }
  for (int i = 0; i < THREADS; i+=2){
    pthread_join(threadList[i+1], NULL); //makes thread 1 and 3 null
  }
  merge(arr, left, middle, right);
}
