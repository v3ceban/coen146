// Name: Vladimir Ceban
// Date: Jan. 16, 2024
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

// data structure to hold copy
typedef struct copy_struct {
  int thread_id;
  char *src_filename;
  char *dst_filename;
} COPY;

// copies from src_filename to dst_filename using functions fopen(),
// fread(), fwrite(), fclose()
static int func_copy(char *src_file, char *dst_file) {
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

// thread function to copy one file
static void *copy_thread(void *arg) {
  COPY params = *(COPY *)arg; // cast/dereference void* to copy_struct
  printf("thread[%i] - copying %s to %s\n", params.thread_id,
         params.src_filename, params.dst_filename);
  // call file copy function
  func_copy(params.src_filename, params.dst_filename);

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if (argc < 3) { // check correct usage
    fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
    exit(1);
  }

  int i;
  char *src_filename;
  char *dst_filename;
  int num_threads = (argc - 1) / 2; // number of threads to create
  // char *src_filenames[num_threads]; // array of source files
  // char *dst_filenames[num_threads]; // array of desintation files

  pthread_t threads[num_threads];  // initialize threads
  COPY thread_params[num_threads]; // structure for each thread

  // initialize thread parameters and create each copy thread
  for (i = 0; i < num_threads; i++) {
    src_filename = argv[i * 2 + 1];
    dst_filename = argv[i * 2 + 2];
    // src_filenames[i] = src_filename;
    // dst_filenames[i] = dst_filename;

    thread_params[i].thread_id = i;
    thread_params[i].src_filename = src_filename;
    thread_params[i].dst_filename = dst_filename;

    pthread_create(&threads[i], NULL, copy_thread, (void *)&thread_params[i]);
  }

  // wait for all threads to finish
  for (i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_exit(NULL);
}
