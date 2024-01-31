// UDP Server
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  // Get from the command line port#
  if (argc != 3) {
    printf("Usage: %s <port #> <file name>\n", argv[0]);
    exit(0);
  }
  // Declare socket file descriptor. All Unix I/O streams are referenced by
  // descriptors
  int sockfd;

  // Declare receiving buffer of size 1k bytes
  char buffer[MAX_BUFFER_SIZE];

  // Declare server address to which to bind for receiving messages and client
  // address to fill in sending address
  struct sockaddr_in servAddr, clienAddr;
  socklen_t addrLen = sizeof(struct sockaddr);

  // Open a UDP socket, if successful, returns a descriptor associated with an
  // endpoint
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
  }

  // Setup the server address to bind using socket addressing structure
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[1])); // Port 5000 is assigned
  servAddr.sin_addr.s_addr =
      INADDR_ANY; // Local IP address of any interface is assigned (generally
                  // one interface IP address)

  // Set address/port of server endpoint for socket socket descriptor
  if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) <
      0) {
    perror("Failure to bind server address to the endpoint socket");
    exit(1);
  } else {
    printf("Started server on port %d. Waiting for connection\n",
           atoi(argv[1]));
  }

  FILE *file = fopen(argv[2], "wb");
  if (file == NULL) {
    perror("Failure to open file");
    exit(1);
  }

  // Sever continuously waits for messages from client, then prints incoming
  // messages.
  int numPackets = 0;
  while (1) {
    int nr = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0,
                      (struct sockaddr *)&clienAddr, &addrLen);
    if (nr < 0) {
      perror("Failure to receive data");
      exit(1);
    }

    printf("Received packet of length: %d\n", nr);
    numPackets++;

    fwrite(buffer, sizeof(char), nr, file);

    if (nr < MAX_BUFFER_SIZE) {
      break;
    }
  }

  printf("Received %d packets total. Transmission finished\n", numPackets);

  // Close file
  fclose(file);

  return 0;
}
