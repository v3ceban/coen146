// Name: Vladimir Ceban
// Date: Jan. 9, 2024
// Title: Lab1 - Step 5 – C program to copy files using functions/syscalls and
// measuring time for each
// Description: Copies a files from src_filename to dst_filename using functions
// fopen(), fread(), fwrite(), fclose(). Copies a files form src_filename to
// dst_filename using syscalls open(), read(), write(), close(). Checks the
// runtime to copy a file using functions vs syscalls using clock()

#include <errno.h>    // errno
#include <fcntl.h>    // open(), close()
#include <pthread.h>  // pthread_create()
#include <stdio.h>    // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>   // malloc(), free()
#include <sys/mman.h> // mmap()
#include <time.h>     // clock()
#include <unistd.h>   // read(), write()

#define BUF_SIZE 2048 // buffer size

int sys_copy(char *argv1, char *argv2) {
  int src_fd = open(argv1, O_RDONLY); // opens a file for reading
  if (src_fd < 0) {                   // open() error checking
    fprintf(stderr, "unable to open %s for reading: %i\n", argv1, errno);
    close(src_fd);
    exit(0);
  }
  // O_WRONLY - write only
  // O_CREAT - create file if it doesn't exist
  // O_TRUNC - truncates file to length 0 if it already exists (deletes existing
  // file) opens a file for writing; erases old file/creates a new file
  int dst_fd = open(argv2, O_WRONLY | O_CREAT | O_TRUNC);
  if (dst_fd < 0) { // open() error checking
    fprintf(stderr, "unable to open/create %s for writing: %i\n", argv2, errno);
    close(dst_fd);
    exit(0);
  }

  // read/ write loop
  void *buf = malloc((size_t)BUF_SIZE); // allocate a buffer to store read data
  int bytes_read = 0;
  while ((bytes_read = read(src_fd, buf, sizeof(buf))) > 0) {
    // reads up to BUF_SIZE bytes from src_filename
    write(dst_fd, buf, bytes_read);
    // writes bytes_read to dst_filename
  }

  close(src_fd);
  // closes src_fd file descriptor
  close(dst_fd);
  // closes dst_fd file descriptor
  free(buf);
  // frees memory used for buf

  return 0;
}

int func_copy(char *argv1, char *argv2) {

  FILE *src = fopen(argv1, "r"); // opens a file for reading
  if (src == NULL) {             // fopen() error checking
    fprintf(stderr, "unable to open %s for reading: %i\n", argv1, errno);
    exit(0);
  }
  FILE *dst = fopen(
      argv2,
      "w"); // opens a file for writing; erases old file/creates a new file
  if (dst == NULL) { // fopen() error checking
    fprintf(stderr, "unable to open/create %s for writing: %i\n", argv2, errno);
    exit(0);
  }

  void *buf = malloc((size_t)BUF_SIZE);
  // allocate a buffer to store read data
  int bytes_read = 0;
  while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0) {
    // reads content of file is loop iterations until end of file
    fwrite(buf, 1, bytes_read, dst);
    // writes bytes_read to dst_filename
  }

  fclose(src);
  // closes src file pointer
  fclose(dst);
  // closes dst file pointer
  free(buf);
  // frees memory used for buf
  return 0;
}

void check_copy_times(char *argv1, char *argv2) {
  clock_t func_start, func_end, syscall_start, syscall_end;

  // capture runtime of func_copy() using start clock, call the copy, end the
  // clock
  double func_time_start = clock();
  func_copy(argv1, argv2);
  double func_time_end = clock();
  double func_time = (func_time_end - func_time_start) / CLOCKS_PER_SEC;

  // capture runtime of syscall_copy() using start clock, call the copy, end the
  // clock
  double sys_time_start = clock();
  sys_copy(argv1, argv2);
  double sys_time_end = clock();
  double syscall_time = (sys_time_end - sys_time_start) / CLOCKS_PER_SEC;

  printf("time to copy using functions: %.7f\n", func_time);
  printf("time to copy using syscalls: %.7f\n", syscall_time);
}

int main(int argc, char *argv[]) {
  if (argc != 3) { // check correct usage
    fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
    exit(1);
  }
  check_copy_times(argv[1], argv[2]);
  return 0;
}
