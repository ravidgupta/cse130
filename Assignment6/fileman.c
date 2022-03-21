/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h> 
#include <stdbool.h>

/*
 * Extended ASCII box drawing characters:
 * 
 * The following code:
 * 
 * printf("CSE130\n");
 * printf("%s%s Assignments\n", TEE, HOR);
 * printf("%s  %s%s Assignment 1\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 2\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 3\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 4\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 5\n", VER, TEE, HOR);
 * printf("%s%s Labs\n", ELB, HOR);
 * printf("   %s%s Lab 1\n", TEE, HOR);
 * printf("   %s%s Lab 2\n", TEE, HOR);
 * printf("   %s%s Lab 3\n", ELB, HOR);
 * printf();
 * 
 * Shows this tree:
 * 
 * CSE130
 * ├─ Assignments
 * │  ├─ Assignment 1
 * │  ├─ Assignment 2
 * │  ├─ Assignment 3
 * │  ├─ Assignment 4
 * │  ├─ Assignment 5
 * └─ Labs
 *    ├─ Lab 1
 *    ├─ Lab 2
 *    └─ Lab 3
 */
#define TEE "\u251C"  // ├ 
#define HOR "\u2500"  // ─ 
#define VER "\u2502"  // │
#define ELB "\u2514"  // └

/*
 * Read at most SIZE bytes from FNAME starting at FOFFSET into BUF starting 
 * at BOFFSET.
 *
 * RETURN number of bytes read from FNAME into BUF, -1 on error.
 */
int fileman_read(
    const char *fname, 
    const size_t foffset, 
    const char *buf, 
    const size_t boffset, 
    const size_t size) 
{
  int readF = 0;
  int fd = open(fname, O_RDONLY); 
  if (fd < 0){
    return -1;
  }
  lseek(fd, foffset, SEEK_CUR);
  readF = read(fd, (void *)(buf + boffset), size);
  close(fd);
  return readF;
}

/*
 * Create FNAME and write SIZE bytes from BUF starting at BOFFSET into FNAME
 * starting at FOFFSET.
 * 
 * RETURN number of bytes from BUF written to FNAME, -1 on error or if FILE 
 * already exists
 */
int fileman_write(
    const char *fname, 
    const size_t foffset, 
    const char *buf, 
    const size_t boffset, 
    const size_t size) 
{
  int writeF = 0; 
  int fd = open(fname, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);//got the end of this statement from a friend.
  if (fd < 0){ 
    return -1; 
  }
  lseek(fd, foffset, SEEK_CUR);
  writeF = write(fd, (void *)(buf + boffset), size);
  close(fd);
  return writeF; 
}

/*
 * Append SIZE bytes from BUF to existing FNAME.
 * 
 * RETURN number of bytes from BUF appended to FNAME, -1 on error or if FNAME
 * does not exist
 */
int fileman_append(const char *fname, const char *buf, const size_t size) 
{
  int appendF = 0;
  int fd = open(fname, O_APPEND | O_WRONLY); 
  if (fd < 0){
    return -1;
  }
  appendF = write(fd, buf, size);
  close(fd);
  return appendF;
}

/*
 * Copy existing file FSRC to new file FDEST.
 *
 * Do not assume anything about the size of FSRC. 
 * 
 * RETURN number of bytes from FSRC written to FDEST, -1 on error, or if FSRC 
 * does not exists, or if SDEST already exists.
 */
int fileman_copy(const char *fsrc, const char *fdest) 
{
  char buffer[1024];
  int count = 0;
  int fd_src = open(fsrc, O_RDONLY);
  int size = 1024;
  int srcF = 1024;
  int destF;
  if (fd_src < 0){
    return -1;
  }
  int fd_dest = open(fdest, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);
  if (fd_dest < 0){
    return -1; 
  }
  while (srcF == size){
    srcF = read(fd_src, (void *)buffer, size);
    destF = write(fd_dest, (void *)buffer, srcF);
    count += destF;
  }
  close(fd_src);
  close(fd_dest);
  return count;
}

/*
 * Print a hierachival directory view starting at DNAME to file descriptor FD 
 * as shown in an example below where DNAME == 'data.dir'
 *
 *   data.dir
 *       blbcbuvjjko
 *           lgvoz
 *               jfwbv
 *                   jqlbbb
 *                   yfgwpvax
 *           tcx
 *               jbjfwbv
 *                   demvlgq
 *                   us
 *               zss
 *                   jfwbv
 *                       ahfamnz
 *       vkhqmgwsgd
 *           agmugje
 *               surxeb
 *                   dyjxfseur
 *                   wy
 *           tcx
 */

static int compare(const void *a, const void *b)
{
  struct dirent **dir1 = (struct dirent **)a;
  struct dirent **dir2 = (struct dirent **)b;
  return strcmp((*dir1)->d_name, (*dir2)->d_name);
}

static char pathBuf[256];

static void get_dir(const int fd, const char *dname, int indent)
{
  struct dirent *dirStream[32];
  int count = 0;
  if (indent > 0){
    sprintf(pathBuf, "%*c%s\n", indent, ' ', dname);
  } 
  else{
    sprintf(pathBuf,"%s\n",dname);
  }
  write(fd, pathBuf, strlen(pathBuf));
  DIR *dir = opendir(dname);
  printf("d: %p\n", (void *)dir);
  if (dir){
    struct dirent *strDir;
    while ((strDir = readdir(dir)) != NULL){
      if (strcmp(strDir->d_name, ".") == 0 || strcmp(strDir->d_name, "..") == 0){
        continue;
      }
      dirStream[count] = strDir;
      count++;
    }
  }
  qsort(dirStream, count, sizeof(struct dirent *), compare);
  chdir(dname);//copied from friend
  for (int i = 0; i < count; i++){
    if (dirStream[i]->d_type != 4){
      sprintf(pathBuf, "%*c%s\n", indent + 4, ' ', dirStream[i]->d_name);
      write(fd, pathBuf, strlen(pathBuf));
    }
    else{
      get_dir(fd, dirStream[i]->d_name, indent + 4);
    }
  }
  chdir("..");//copied from friend
  closedir(dir);
}


void fileman_dir(const int fd, const char *dname) 
{
  get_dir(fd, dname, 0);
}



/*
 * Print a hierachival directory tree view starting at DNAME to file descriptor 
 * FD as shown in an example below where DNAME == 'data.dir'.
 * 
 * Use the extended ASCII box drawing characters TEE, HOR, VER, and ELB.
 *
 *   data.dir
 *   ├── blbcbuvjjko
 *   │   ├── lgvoz
 *   │   │   └── jfwbv
 *   │   │       ├── jqlbbb
 *   │   │       └── yfgwpvax
 *   │   └── tcx
 *   │       ├── jbjfwbv
 *   │       │   ├── demvlgq
 *   │       │   └── us
 *   │       └── zss
 *   │           └── jfwbv
 *   │               └── ahfamnz
 *   └── vkhqmgwsgd
 *       ├── agmugje
 *       │   └── surxeb
 *       │       ├── dyjxfseur
 *       │       └── wy
 *       └── tcx
 */
void fileman_tree(const int fd, const char *dname) 
{
}
