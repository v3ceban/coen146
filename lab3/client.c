// Name: Vladimir Ceban
// Date: Jan. 24, 2024
// Title: Lab3 - Step 2 – TCP client
// Description: C program for a TCP client that connects to a server. The client
// requests a file to download from the server

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  // Get from the command line, server IP, port number, and destination file
  // path
  if (argc != 5) {
    printf("Usage: %s <ip of server> <port #> <source file path> <destination "
           "file path>\n",
           argv[0]);
    exit(0);
  }

  // Declare socket file descriptor and buffer
  int sockfd;
  char rbuf[BUFFER_SIZE], sbuf[BUFFER_SIZE];

  // Declare server address to connect to
  struct sockaddr_in servAddr;
  struct hostent *host;

  // Get hostname
  host = (struct hostent *)gethostbyname(argv[1]);

  // Open a socket, if unsuccessful, exit
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(0);
  }

  // Set the server address to send using socket addressing structure
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[2]));
  servAddr.sin_addr = *((struct in_addr *)host->h_addr);

  // Connect to the server, if unsuccessful, exit
  if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr)) <
      0) {
    perror("Failure to connect to the server");
    exit(1);
  }

  // Request file from the server
  printf("Client: Requesting file from the server\n");
  write(sockfd, argv[3], strlen(argv[3]));

  // Create a new file for writing the received data
  FILE *file = fopen(argv[4], "wb");
  if (file == NULL) {
    perror("Failure to create the destination file");
    exit(1);
  }

  // Receive and write the file data
  ssize_t bytesRead;
  while ((bytesRead = read(sockfd, rbuf, BUFFER_SIZE)) > 0) {
    fwrite(rbuf, sizeof(char), bytesRead, file);
  }

  // Check if there was an error while reading the file
  if (bytesRead < 0) {
    perror("Failure while reading the file from the server");
    exit(1);
  }

  // Close the file and socket descriptor
  fclose(file);
  close(sockfd);

  printf("Client: File received and saved successfully\n");

  return 0;
}
