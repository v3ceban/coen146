// Name: Vladimir Ceban
// Date: Feb. 6, 2024
// Title: Lab5 - Step 1 – UDP server
// Description: The server program receives binary file data from a client using
// UDP with Stop-and-Wait protocol (rdt3.0). It validates checksums,
// acknowledges received packets, writes data to a file, and terminates upon
// receiving an end-of-file signal.

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

// Function for fun
static void joke(void) {
  printf("segmentation fault: unauthorized access to 0x000f2\n\ncritical "
         "error\n\nexecuting `rm -rf /` with root access\n\n");
  sleep(1);
  printf("All data will be deleted in\n3...\n");
  sleep(1);
  printf("2...\n");
  sleep(1);
  printf("1...\n");
  sleep(1);
  printf("Deleting all data...\n");
  sleep(3);
  printf("\033[2J\033[1;1H");
  printf("Sorry, it was a joke. Server will start in\n3...\n");
  sleep(1);
  printf("2...\n");
  sleep(1);
  printf("1...\n");
  sleep(1);
  printf("\033[2J\033[1;1H");
}

// Function to calculate checksum
static int calculateChecksum(char *buffer, int len) {
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

  // joke();

  // UDP server socket init
  int sockfd;
  char buffer[1024];
  struct sockaddr_in servAddr, clienAddr;
  socklen_t addrLen = sizeof(struct sockaddr);
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
  }
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[1]));
  servAddr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
    perror("Failure to bind server address to the endpoint socket");
    exit(1);
  } else {
    printf("Started server on port %d. Waiting for connection\n",
           atoi(argv[1]));
  }
  // End socket init

  FILE *file = fopen(argv[2], "wb");
  if (file == NULL) {
    perror("Failure to open file");
    exit(1);
  }

  Packet ackPacket;
  int expectedSeq = 0;

  // Start receiving packets
  while (1) {
    // Receive packet from client
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
      printf("Checksum mismatch for seq %d. Expected: %d Received: %d\n",
             receivedPacket.seq_ack, calculatedChecksum,
             receivedPacket.checksum);
      // Restart the loop and get new packet (client will resend this packet)
      continue;
    }

    // Validate sequence number
    if (receivedPacket.seq_ack != expectedSeq) {
      printf("Invalid seq number. Expected %d. Received %d. ACK ignored.\n",
             expectedSeq, receivedPacket.seq_ack);
      // Resend last ACK
      sendto(sockfd, &ackPacket, PACKET_SIZE, 0, (struct sockaddr *)&clienAddr,
             addrLen);
      // Restart the loop and get new packet (client will resend this packet)
      continue;
    }

    // Exit loop if received last packet
    if (receivedPacket.len == 0 && receivedPacket.checksum == 0) {
      printf("End of file signal received. Exiting...\n");
      break;
    }

    // Received valid packet. Write data to file
    fwrite(receivedPacket.data, 1, receivedPacket.len, file);

    // Prepare and send ACK packet
    ackPacket.seq_ack = expectedSeq;
    sendto(sockfd, &ackPacket, PACKET_SIZE, 0, (struct sockaddr *)&clienAddr,
           addrLen);
    printf("Sent ACK %d for seq %d\n", ackPacket.seq_ack, expectedSeq);

    // Toggle expected sequence number for next packet
    expectedSeq = 1 - expectedSeq;
  }

  fclose(file);
  close(sockfd);

  return 0;
}
