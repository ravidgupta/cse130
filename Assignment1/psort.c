/************************************************************************
 * 
 * CSE130 Assignment 1
 *  
 * POSIX Shared Memory Multi-Process Merge Sort
 * 
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ************************************************************************/
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "merge.h"

/* 
 * Merge Sort in the current process a sub-array of ARR[] defined by the 
 * LEFT and RIGHT indexes.
 */
void singleProcessMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleProcessMergeSort(arr, left, middle); 
    singleProcessMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 * Merge Sort in the current process and at least one child process a 
 * sub-array of ARR[] defined by the LEFT and RIGHT indexes.
 */
void multiProcessMergeSort(int arr[], int left, int right) 
{
  int shmid = shm_open("RAD", O_CREAT | O_RDWR, 0666);//taken from GeeksforGeeks
  ftruncate(shmid,  1024);//taken from GeeksforGeeks
  int *str = mmap(0, 1024, PROT_WRITE, MAP_SHARED, shmid, 0);//taken from GeekforGeeks
  int middle = (left+right)/2;
  int size = right + 1;
  memcpy(str, arr, sizeof(int) * size);//copy into shared memory
  switch(fork()){
    case -1:
      exit(-1);//exit failure if case -1
    case 0:
      singleProcessMergeSort(str, middle + 1, right);//sort right side
      exit(0);
    default:
      singleProcessMergeSort(arr, left, middle);//sort left side
      wait(NULL);
      for (int i = middle + 1; i < right + 1; i++){//get right side from shared mem
         arr[i] = str[i];
      }
      merge(arr, left, middle, right); //merge local mem
      munmap(str, 1024);//taken from linuxhint.com
      close(shmid);//taken from linuxhint.com
  }

}
