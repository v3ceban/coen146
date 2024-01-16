// Name: Vladimir Ceban
// Date: Jan. 17, 2024
// Title: Lab2 - Step 1 – C program to copy files using threads
// Description: Receives a nummber of files, creates a number of threads and
// copies files from src_filename to dst_filename using functions fopen(),
// fread(), fwrite(), fclose().

#include <errno.h>    // errno
#include <fcntl.h>    // open(), close()
#include <pthread.h>  // pthread_create()
#include <stdio.h>    // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>   // malloc(), free()
#include <sys/mman.h> // mmap()
#include <time.h>     // clock()
#include <unistd.h>   // read(), write()

#define BUF_SIZE 2048 // buffer size

// copies from src_filename to dst_filename using functions fopen(),
// fread(), fwrite(), fclose()
int func_copy(char *src_file, char *dst_file) {
  FILE *src = fopen(src_file, "r"); // opens a file for reading
  if (src == NULL) {                // fopen() error checking
    fprintf(stderr, "unable to open %s for reading: %i\n", src_file, errno);
    exit(0);
  }

  FILE *dst = fopen(dst_file, "w");
  // opens a file for writing; erases old file/creates a new file
  if (dst == NULL) { // fopen() error checking
    fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_file,
            errno);
    exit(0);
  }

  void *buf = malloc((size_t)BUF_SIZE); // allocate a buffer to store read data
  int bytes_read = 0;
  while ((bytes_read = fread(buf, 1, sizeof(buf), src)) > 0) {
    // reads content of file is loop iterations until end of file
    fwrite(buf, 1, bytes_read, dst); // writes bytes_read to dst_filename
  }

  fclose(src); // closes src file pointer
  fclose(dst); // closes dst file pointer
  free(buf);   // frees memory used for buf
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 3) { // check correct usage
    fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
    exit(1);
  }
  func_copy(argv[1], argv[2]);
  return 0;
}
