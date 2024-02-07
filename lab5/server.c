// Name: Vladimir Ceban
// Date: Feb. 6, 2024
// Title: Lab5 - Step 1 – UDP server
// Description: C program for a UDP server that awaits a connection from the
// host and sends it a file it requests

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PACKET_SIZE                                                            \
  1037 // Total size of the packet: header (12 bytes) + data (1024 bytes) + 1
       // byte for null terminator
#define DATA_SIZE 1024 // Size of data buffer in the packet

typedef struct {
  int seq_ack;
  int len;
  int checksum;
  char data[DATA_SIZE];
} Packet;

// Function to calculate checksum
int calculateChecksum(char *buffer, int len) {
  int checksum = 0;
  for (int i = 0; i < len; i++) {
    checksum ^= buffer[i];
  }
  return checksum;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <port #> <file name>\n", argv[0]);
    exit(1);
  }

  int sockfd;
  char buffer[1024];
  struct sockaddr_in servAddr, clienAddr;
  socklen_t addrLen = sizeof(struct sockaddr);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
  }

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[1])); // Port 5000 is assigned
  servAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
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

  Packet ackPacket;
  int expectedSeq = 0;

  while (1) {
    Packet receivedPacket;
    if (recvfrom(sockfd, &receivedPacket, PACKET_SIZE, 0,
                 (struct sockaddr *)&clienAddr, &addrLen) < 0) {
      perror("Error in receiving packet");
      exit(1);
    }

    // Validate checksum
    int calculatedChecksum =
        calculateChecksum(receivedPacket.data, receivedPacket.len);
    if (receivedPacket.checksum != calculatedChecksum) {
      printf("Checksum mismatch. Expected: %d. Received: %d. Packet ignored.\n",
             receivedPacket.checksum, calculatedChecksum);
      continue;
    }

    // Validate sequence number
    if (receivedPacket.seq_ack != expectedSeq) {
      printf(
          "Invalid sequence number. Expected: %d. Received: %d. ACK ignored.\n",
          expectedSeq, receivedPacket.seq_ack);
      // Resend last ACK
      sendto(sockfd, &ackPacket, PACKET_SIZE, 0, (struct sockaddr *)&clienAddr,
             addrLen);
      continue;
    }

    if (receivedPacket.len == 0) {
      printf("End of file signal received. Exiting...\n");
      break;
    }

    // Write data to file
    fwrite(receivedPacket.data, 1, receivedPacket.len, file);
    fflush(file);

    // Prepare ACK packet
    ackPacket.seq_ack = expectedSeq;
    ackPacket.len = 0;
    ackPacket.checksum = calculateChecksum("", 0);

    // Send ACK
    sendto(sockfd, &ackPacket, PACKET_SIZE, 0, (struct sockaddr *)&clienAddr,
           addrLen);
    printf("Sent ACK %d for seq: %d\n", ackPacket.seq_ack, expectedSeq);

    // Toggle sequence number
    expectedSeq = 1 - expectedSeq;
  }

  fclose(file);
  close(sockfd);

  return 0;
}
