// Name: Vladimir Ceban
// Date: Feb. 6, 2024
// Title: Lab5 - Step 2 – UDP client
// Description: C program for a UDP server that connects to a server and sends a
// file to it.

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  if (rand() % 100 < 20) {
    return -1;
  }

  return checksum;
}

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
  Packet packet;
  int seq_ack = 0;
  int receivedSeqAck = -1;
  int count = 0;

  while ((packet.len = fread(packet.buffer, 1, sizeof(packet.buffer), file)) >
         0) {
    packet.seq_ack = seq_ack;
    packet.checksum = calculateChecksum(packet.buffer, packet.len);

    // Send the packet to the server
    sendto(sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&servAddr,
           sizeof(struct sockaddr));

    // Receive the seq_ack from the server
    recvfrom(sockfd, &receivedSeqAck, sizeof(int), 0,
             (struct sockaddr *)&servAddr, &addrLen);

    // Print the seq_ack
    printf("%d. Sent seq: %d. Received ack: %d\n", count++, packet.seq_ack,
           receivedSeqAck);

    // Resend the packet if received seq_ack is not the same as previously sent
    // one
    if (receivedSeqAck != seq_ack) {
      sendto(sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&servAddr,
             sizeof(struct sockaddr));
    }

    // Update seq_ack for the next packet
    if (receivedSeqAck == 0) {
      seq_ack = 1;
    } else {
      seq_ack = 0;
    }
  }

  // Close the file
  fclose(file);

  return 0;
}
