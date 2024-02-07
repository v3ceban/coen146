// Name: Vladimir Ceban
// Date: Feb. 6, 2024
// Title: Lab5 - Step 1 – UDP server
// Description: C program for a UDP server that awaits a connection from the
// host and sends it a file it requests

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

typedef struct {
  int seq_ack;
  int len;
  int checksum;
  char buffer[1024];
} Packet;

int calculateChecksum(char *buffer, int len) {
  int checksum = 0;
  for (int i = 0; i < len; i++) {
    checksum ^= buffer[i];
  }
  return checksum;
}

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
  char buffer[1024];

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
  while (1) {
    Packet packet;
    int pr = recvfrom(sockfd, &packet, sizeof(Packet), 0,
                      (struct sockaddr *)&clienAddr, &addrLen);
    if (pr < 0) {
      perror("Failure to receive data");
      exit(1);
    }

    printf("Received packet with seq: %d, len: %d, checksum: %d\n",
           packet.seq_ack, packet.len, packet.checksum);

    int calculatedChecksum = calculateChecksum(packet.buffer, packet.len);
    if (packet.checksum != calculatedChecksum || packet.len == 0) {
      printf(
          "Something went wrong.\nCalculated checksum: %d. Packet length: %d\n",
          calculatedChecksum, packet.len);
      if (packet.seq_ack == 0) {
        packet.seq_ack = 1;
      } else {
        packet.seq_ack = 0;
      }
    } else {
      fwrite(packet.buffer, sizeof(char), packet.len, file);
    }

    int ps = sendto(sockfd, &packet, sizeof(Packet), 0,
                    (struct sockaddr *)&clienAddr, addrLen);
    if (ps < 0) {
      perror("Failure to send data");
      exit(1);
    }
    if (packet.len < 1024) {
      break;
    }
  }

  printf("Transmission finished\n");

  // Close file
  fclose(file);

  return 0;
}
