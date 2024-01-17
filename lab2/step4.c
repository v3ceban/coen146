// Name: Vladimir Ceban
// Date: Jan. 16, 2024
// Title: Lab2 - Step 4
// Description: C program to calculate RTT times for Persistent and
// Non-Persistent HTML connections

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) { // check correct usage
    fprintf(stderr, "usage: %s <n connections> \n", argv[0]);
    exit(1);
  }
  // Declare and values to n (n connections), np (np parralel connections),
  // RTT0, RTT1, RTT2, RTTHTTP, RTTDNS, ..
  int rtt0 = 3;
  int rtt1 = 20;
  int rtt2 = 26;
  int rtthttp = 47;
  int n, np;

  printf("One object: %d msec\n", rtt0 + rtt1 + rtt2 + rtthttp);
  printf("Non-Persistent 6 objects: %d msec\n",
         6 * (rtt0 + rtt1 + rtt2 + rtthttp));

  // find how many np (parralel connections)
  np = atoi(argv[1]);

  printf("%d parallel connection - Persistent: %d msec\n", np,
         rtt0 + rtt1 + rtt2 + rtthttp);
  printf("%d parallel connection - Non-Persistent: %d msec\n", np,
         np * (rtt0 + rtt1 + rtt2 + rtthttp));

  return 0;
}
