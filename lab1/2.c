// Name: Vladimir Ceban
// Date: Jan. 9, 2024
// Title: Lab1 - Step 4 – C program to copy files using syscalls
// Description: Copies a files form src_filename to dst_filename using syscalls
// open(), read(), write(), close()

#include <errno.h>    // errno
#include <fcntl.h>    // open(), close()
#include <pthread.h>  // pthread_create()
#include <stdio.h>    // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>   // malloc(), free()
#include <sys/mman.h> // mmap()
#include <unistd.h>   // read(), write()

#define BUF_SIZE 2048 // buffer size

int main(int argc, char *argv[]) {
  int src_fd = open(argv[1], O_RDONLY); // opens a file for reading
  if (src_fd < 0) {                     // open() error checking
    fprintf(stderr, "unable to open %s for reading: %i\n", argv[1], errno);
    close(src_fd);
    exit(0);
  }
  // O_WRONLY - write only
  // O_CREAT - create file if it doesn't exist
  // O_TRUNC - truncates file to length 0 if it already exists (deletes existing
  // file) opens a file for writing; erases old file/creates a new file
  int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
  if (dst_fd < 0) { // open() error checking
    fprintf(stderr, "unable to open/create %s for writing: %i\n", argv[2],
            errno);
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
