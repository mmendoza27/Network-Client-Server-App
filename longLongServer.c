#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "uici.h"
#include "primelib.h"
#include "restart.h"

int main(int argc, char *argv[]) {
   char client[MAX_CANON];
   char *isPrime;
   int answer;
   int communicationFD;
   int listeningFD;
   long long int numberToTest;
   u_port_t portNumber;

   if(argc != 2) {
      fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
      return 1;
   }

   portNumber = (u_port_t) atoi(argv[1]);

   if((listeningFD = u_open(portNumber)) == -1) {
      perror("Failed to create a listening endpoint");
      return 1;
   }

   for( ; ; ) {
      if((communicationFD = u_accept(listeningFD, client, MAX_CANON)) == -1) {
         perror("Failed to accept connection");
         continue;
      }

      if((isPrime = (char *) calloc (1, sizeof(char))) == NULL) {
         fprintf(stderr, "Could not allocate memory");
         return 1;
      }

      if(r_read(communicationFD, &numberToTest, sizeof(numberToTest)) == -1)
         perror("Failed to read from the communication file descriptor");

      if(isPrimeLongLong(numberToTest) == 1)
         isPrime[0] = '1';         
      else
         isPrime[0] = '0';

      if(r_write(communicationFD, isPrime, 1) == -1)
         perror("Failed to write to the communication file descriptor");

      if(r_close(communicationFD) == -1)
         perror("Failed to close communication file descriptor");

   }

   return 0;
}