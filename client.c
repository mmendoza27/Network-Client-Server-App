#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "restart.h"
#include "uici.h"

int main(int argc, char *argv[]) {
   char *buffer;
   char *buffer2;
   int communicationFD;
   u_port_t portNumber;

   if(argc != 4) {
      fprintf(stderr, "Usage: %s PrimeNumber PortNumber Hostname\n", argv[0]);
      return 1;
   }

   if((buffer = (char *) calloc (21, sizeof(char))) == NULL){
      fprintf(stderr, "Could not allocate memory");
      return 1;
   }

   if((buffer2 = (char *) calloc (1, sizeof(char))) == NULL) {
      fprintf(stderr, "Could not allocate memory");
      return 1;
   }

   portNumber = (u_port_t) atoi(argv[2]);

   if((communicationFD = u_connect(portNumber, argv[3])) == -1) {
      perror("Failed to make a connection");
      return 1;
   }

   buffer = argv[1];
   buffer[(strlen(buffer) + 1)] = '\0';

   if(r_write(communicationFD, buffer, 21) == -1)
      perror("Failed to write to the communication file descriptor");

   if(r_read(communicationFD, buffer2, 1) == -1)
      perror("Failed to read from the communication file descriptor");

   if(strcmp(buffer2, "1") == 0)
      printf("%s is prime.\n", buffer);
   else
      printf("%s is not prime.\n", buffer);

   return 0;
}