// Name: Vladimir Ceban
// Date: Feb. 6, 2024
// Title: Lab5 - Step 2 – UDP client
// Description: C program for a UDP server that connects to a server and sends a
// file to it.

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
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

int calculateChecksum(char *buffer, int len) {
  int checksum = 0;
  for (int i = 0; i < len; i++) {
    checksum ^= buffer[i];
  }
  if (rand() % 100 < 20) {
    return 0;
  }
  return checksum;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Usage: %s <ip of server> <port #> <file name>\n", argv[0]);
    exit(1);
  }

  int sockfd;
  struct sockaddr_in servAddr;
  socklen_t addrLen = sizeof(struct sockaddr);
  struct hostent *host;
  host = (struct hostent *)gethostbyname(argv[1]);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
  }

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(argv[2]));
  servAddr.sin_addr = *((struct in_addr *)host->h_addr);

  FILE *file = fopen(argv[3], "rb");
  if (file == NULL) {
    perror("Failure to open the file");
    exit(1);
  }

  Packet packet;
  int seq = 0;
  int lastPacketSent = 0;
  int resendCounter = 0;

  while (!feof(file)) {
    memset(&packet, 0, sizeof(Packet));
    packet.seq_ack = seq;
    packet.len = fread(packet.data, 1, DATA_SIZE, file);
    packet.checksum = calculateChecksum(packet.data, packet.len);
    if (sendto(sockfd, &packet, PACKET_SIZE, 0, (struct sockaddr *)&servAddr,
               addrLen) < 0) {
      perror("Error in sending packet");
      exit(1);
    }
    lastPacketSent = seq;
    printf("Sent packet with seq: %d, checksum: %d\n", seq, packet.checksum);
    // Set a timeout for receiving ACK
    struct timeval tv;
    tv.tv_sec = 0;    // Timeout value in seconds
    tv.tv_usec = 500; // Timeout value in miliseconds

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
               sizeof(struct timeval));

    // Wait for ACK
    Packet ackPacket;
    if (recvfrom(sockfd, &ackPacket, PACKET_SIZE, 0, NULL, NULL) < 0) {
      // Timeout occurred, resend the packet
      printf("Timeout occurred. Resending packet with seq: %d\n",
             lastPacketSent);
      resendCounter++;
      if (resendCounter > 3) {
        printf("Maximum retransmission attempts reached. Exiting...\n");
        exit(1);
      }
      fseek(file, -packet.len,
            SEEK_CUR); // Rollback file pointer to resend the packet
    } else {
      // Check if received ACK is valid
      if (ackPacket.seq_ack == seq && ackPacket.len == 0 &&
          ackPacket.checksum == calculateChecksum("", 0)) {
        printf("Received ACK %d for seq: %d\n", ackPacket.seq_ack, seq);
        seq = 1 - seq;     // Toggle sequence number
        resendCounter = 0; // Reset resend counter on successful transmission
      } else {
        // Invalid ACK received, ignore and wait for timeout
        printf("Received invalid ACK %d. Ignoring and waiting for timeout.\n",
               ackPacket.seq_ack);
      }
    }
  }

  // Send empty packet to signal end of file
  Packet endPacket;
  endPacket.seq_ack = seq;
  endPacket.len = 0;
  endPacket.checksum = calculateChecksum("", 0);
  sendto(sockfd, &endPacket, PACKET_SIZE, 0, (struct sockaddr *)&servAddr,
         addrLen);

  fclose(file);
  close(sockfd);

  return 0;
}
