// Name: Vladimir Ceban
// Date: Jan. 24, 2024
// Title: Lab3 - Step 1 – TCP server
// Description: C program for a TCP server that accepts a client connection. The
// server transfers a file to the client

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  // Get the port number from the command line
  if (argc != 2) {
    printf("Usage: %s <port #> \n", argv[0]);
    exit(0);
  }

  // Declare socket file descriptor
  int sockfd, connfd, rb, sin_size;

  // Declare receiving and sending buffers of size 1k bytes
  char rbuf[1024], sbuf[1024];

  // Declare server address to bind for receiving messages and client address to
  // fill in sending address
  struct sockaddr_in servAddr, clienAddr;

  // Open a TCP socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
  }

  // Setup the server address to bind using socket addressing structure
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[1]));
  servAddr.sin_addr.s_addr = INADDR_ANY;

  // Set address/port of server endpoint for socket socket descriptor
  if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) <
      0) {
    perror("Failure to bind server address to the endpoint socket");
    exit(1);
  }

  printf("Server waiting for client at port %d\n", atoi(argv[1]));

  // Server listening to the socket endpoint, and can queue 5 client requests
  listen(sockfd, 5);
  sin_size = sizeof(struct sockaddr_in);

  while (1) {
    // Server accepting to establish a connection with a connecting client
    if ((connfd = accept(sockfd, (struct sockaddr *)&clienAddr,
                         (socklen_t *)&sin_size)) < 0) {
      perror("Failure to accept connection to the client");
      exit(1);
    }

    // Connection established, server begins to read and write to the connecting
    // client
    printf("Connection Established with client: IP %s and Port %d\n",
           inet_ntoa(clienAddr.sin_addr), ntohs(clienAddr.sin_port));

    // Receive the file name from the client
    rb = read(connfd, rbuf, sizeof(rbuf));
    rbuf[rb] = '\0';
    printf("Client requested file: %s\n", rbuf);

    // Open the requested file
    int filefd = open(rbuf, O_RDONLY);
    if (filefd < 0) {
      perror("Failure to open the file");
      write(connfd, "File not found", 15);
      close(connfd);
      continue;
    }

    // Read the file contents and send them to the client
    while ((rb = read(filefd, sbuf, sizeof(sbuf))) > 0) {
      write(connfd, sbuf, rb);
    }

    // Close the file and the connection
    close(filefd);
    close(connfd);
  }

  close(sockfd);
  return 0;
}
