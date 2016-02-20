#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "restart.h"
#include "uici.h"

int main(int argc, char *argv[]) {
   char *buffer;
   int communicationFD;
   long long int number;
   u_port_t portNumber;

   if(argc != 4) {
      fprintf(stderr, "Usage: %s PrimeNumber Hostname PortNumber\n", argv[0]);
      return 1;
   }

   if((buffer = (char *) calloc (1, sizeof(char))) == NULL) {
      fprintf(stderr, "Could not allocate memory");
      return 1;
   }

   portNumber = (u_port_t) atoi(argv[3]);

   if((communicationFD = u_connect(portNumber, argv[2])) == -1) {
      perror("Failed to make a connection");
      return 1;
   }

   number = atoll(argv[1]);

   if(r_write(communicationFD, &number, sizeof(number)) == -1)
      perror("Failed to write to the communication file descriptor");

   if(r_read(communicationFD, buffer, 1) == -1)
      perror("Failed to read from the communication file descriptor");

   if(strcmp(buffer, "1") == 0)
      printf("%lli is prime.\n", number);
   else
      printf("%lli is not prime.\n", number);

   return 0;
}