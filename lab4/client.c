// UDP Client
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  // Get from the command line, server IP, port number, and file name
  if (argc != 4) {
    printf("Usage: %s <ip of server> <port #> <file name>\n", argv[0]);
    exit(0);
  }

  // Declare socket file descriptor. All Unix I/O streams are referenced by
  // descriptors
  int sockfd;

  // Declare server address
  struct sockaddr_in servAddr;
  socklen_t addrLen = sizeof(struct sockaddr);

  // Converts domain names into numerical IP addresses via DNS
  struct hostent *host;
  host = (struct hostent *)gethostbyname(
      argv[1]); // You may use "localhost" or "127.0.0.1": loopback IP address

  // Open a socket, if successful, returns a descriptor associated with an
  // endpoint
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
  }

  // Set the server address to send using socket addressing structure
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[2]));
  servAddr.sin_addr = *((struct in_addr *)host->h_addr);

  // Open the file in binary mode for reading
  FILE *file = fopen(argv[3], "rb");
  if (file == NULL) {
    perror("Failure to open the file");
    exit(1);
  }

  // Read the file and send its contents to the server
  char buffer[1024];
  size_t bytesRead;
  while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    sendto(sockfd, buffer, bytesRead, 0, (struct sockaddr *)&servAddr,
           sizeof(struct sockaddr));
  }

  // Close the file
  fclose(file);

  return 0;
}
