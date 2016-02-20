#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include "restart.h"
#include "uici.h"

int main(int argc, char *argv[]) {
   char *buffer;
   int communicationFD;
   int highestFD;
   int index;
   int primesToFind;
   long long int number;
   fd_set readSet;
   u_port_t portNumber;

   if(argc < 5) {
      fprintf(stderr, "Usage: %s StartingPrime NumberOfPrimesToFind "
         "PortNumber Hostname [Additional HostNames...]\n", argv[0]);
      return 1;
   }

   if((buffer = (char *) calloc (1, sizeof(char))) == NULL) {
      fprintf(stderr, "Could not allocate memory");
      return 1;
   }

   portNumber = (u_port_t) atoi(argv[3]);
   primesToFind = atoi(argv[2]);
   number = atoll(argv[1]);

   FD_ZERO(&readSet);

   for(index = argc - 1; index > 3; index--){
      if((communicationFD = u_connect(portNumber, argv[index])) == -1) {
         perror("Failed to make a connection");
         return 1;
      }

      printf("Made a connection with %s\n", argv[index]);
      FD_SET(communicationFD, &readSet);
   }

   if(r_write(communicationFD, &number, sizeof(number)) == -1)
      perror("Failed to write to the communication file descriptor");

   if(r_read(communicationFD, buffer, 1) == -1)
      perror("Failed to read from the communication file descriptor");

   if(strcmp(buffer, "1") == 0)
      printf("%lli\n", number);


   return 0;
}