// Name: Vladimir Ceban
// Date: Jan. 9, 2024
// Title: Lab1 - Step 3 – C program to copy files using functions
// Description: Copies a files from src_filename to dst_filename using
// functions fopen(), fread(), fwrite(), fclose()

#include <errno.h>    // errno
#include <fcntl.h>    // open(), close()
#include <pthread.h>  // pthread_create()
#include <stdio.h>    // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>   // malloc(), free()
#include <sys/mman.h> // mmap()
#include <time.h>     // clock()
#include <unistd.h>   // read(), write()

#define BUF_SIZE 2048 // buffer size

int main(int argc, char *argv[]) {

  FILE *src = fopen(argv[1], "r"); // opens a file for reading
  if (src == NULL) {               // fopen() error checking
    fprintf(stderr, "unable to open %s for reading: %i\n", argv[0], errno);
    exit(0);
  }
  FILE *dst = fopen(
      argv[2],
      "w"); // opens a file for writing; erases old file/creates a new file
  if (dst == NULL) { // fopen() error checking
    fprintf(stderr, "unable to open/create %s for writing: %i\n", argv[1],
            errno);
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
