#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "restart.h"
#include "uici.h"

#define numberOfThreads 4

static char *buffer;
static int addToList;
static int primesToFind;
static long long int *primes;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static u_port_t portNumber;

static int primeArray[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
   47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };

int compareLongLong(const void *firstValue, const void *secondValue);
int decrementPrimesNeededMutex();
int getNumberMutex(char *buf);
int insertIntoBufferMutex(char *buf);
void *sendNumber(void *arg);

int main(int argc, char *argv[]) {
   char *hostName;
   int index;
   int index2;
   int numberOfHosts;
   int primesToPrint;
   pthread_t *threadIDs;

                                                /* Check number of parameters */
   if(argc < 5) {
      fprintf(stderr, "Usage: %s StartingPrime NumberOfPrimesToFind "
         "PortNumber Hostname [Additional Hostnames...]\n", argv[0]);
      return 1;
   }

                                               /* Allocate memory for buffers */
   if((hostName = (char *) calloc (MAX_CANON, sizeof(char))) == NULL) {
      fprintf(stderr, "Could not allocate memory");
      return 1;
   }

   if((buffer = (char *) calloc (21, sizeof(char))) == NULL){
      fprintf(stderr, "Could not allocate memory");
      return 1;
   }


    /* Initialize number of hosts, port number, number of primes to find, etc */ 
   numberOfHosts = argc - 4;                     /* Used for creating threads */
   portNumber = (u_port_t) atoi(argv[3]);
   primesToPrint = primesToFind = atoi(argv[2]);
   buffer = argv[1];
   buffer[(strlen(buffer) + 1)] = '\0';

   printf("%s\n", argv[1]);

   if((primes = (long long int *) calloc ((numberOfHosts * numberOfThreads) + 
      primesToPrint, sizeof(long long int))) == NULL){
         fprintf(stderr, "Could not allocate memory");
         return 1;
   }

                                            /* Allocate memory for thread IDs */
   threadIDs = (pthread_t *) malloc(4 * numberOfHosts * sizeof(pthread_t *));
   if(threadIDs == NULL) {
      fprintf(stderr, "Error allocating space for %d thread IDs\n",
         numberOfThreads);
      return 1;
   }

                                            /* Create 4 threads for each host */
   for(index = 0; index < numberOfHosts; index++) {
      hostName = argv[index + 4];

      for(index2 = 0; index2 < numberOfThreads; index2++) {
         if(pthread_create(threadIDs + (index * numberOfThreads + index2),
            NULL, sendNumber, hostName)){
               fprintf(stderr, "Error creating thread with ID: %d\n",index2+1);
               return 1;
         }
      }
   }

                                                      /* Join all the threads */
   for(index = 0; index < numberOfHosts * numberOfThreads; index++) {
      if(pthread_join(threadIDs[index], NULL)) {
         fprintf(stderr, "Error creating thread with ID: %d\n", index + 1);
         return 1;
      }
   }

   qsort(primes, primesToPrint + 1, sizeof(long long int), compareLongLong);

   for(index = 0; index < primesToPrint; index++)
      printf("%lli\n", primes[index]);

   return 0;
}

int compareLongLong(const void *firstValue, const void *secondValue){
   const long long int *value1 = (const long long int *) firstValue;
   const long long int *value2 = (const long long int *) secondValue;
   
   return (*value1 > *value2) - (*value1 < *value2);
}

int decrementPrimesNeededMutex() {
   int error;

   if((error = pthread_mutex_lock(&lock)))
      return error;

   primesToFind--;

   return pthread_mutex_unlock(&lock);
}

int getNumberMutex(char *buf) {
   int error;
   long long int nextNumber;

   if((error = pthread_mutex_lock(&lock)))
      return error;

   strcpy(buf, buffer);
   nextNumber = atoll(buffer);
   nextNumber++;
   snprintf(buffer, 21, "%lli", nextNumber);

   return pthread_mutex_unlock(&lock);
}

int insertIntoBufferMutex(char *buf) {
   int error;
   long long int primeFound;

   if((error = pthread_mutex_lock(&lock)))
      return error;

   primeFound = atoll(buf);
   primes[addToList] = primeFound;
   addToList++;

   return pthread_mutex_unlock(&lock);
}

void *sendNumber(void *arg) {
   char *longToTest;
   char *buffer2;
   char *host = arg;
   int divisor;
   int communicationFD;

   if((longToTest = (char *) calloc (21, sizeof(char))) == NULL) {
      fprintf(stderr, "Could not allocate memory");
      return NULL;
   }

                                               /* Allocate memory for buffers */
   if((buffer2 = (char *) calloc (1, sizeof(char))) == NULL) {
      fprintf(stderr, "Could not allocate memory");
      return NULL;
   }

   while(primesToFind > 0) {
      if((communicationFD = u_connect(portNumber, host)) == -1) {
         perror("Failed to make a connection");
         return NULL;
      }

      getNumberMutex(longToTest);
      divisor = 0;

      while(divisor <= 25) {
         if(atoll(longToTest) % primeArray[divisor] == 0) {
            getNumberMutex(longToTest);
            divisor = 0;
            continue;
         }

         divisor++;
      }

      if(r_write(communicationFD, longToTest, 21) == -1)
         perror("Failed to write to the communication file descriptor");

      if(r_read(communicationFD, buffer2, 1) == -1)
         perror("Failed to read from the communication file descriptor");

      if(strcmp(buffer2, "1") == 0){
         insertIntoBufferMutex(longToTest);
         decrementPrimesNeededMutex();         
      }

      if(r_close(communicationFD) == -1)
         perror("Failed to close communication file descriptor");
   }

   free(longToTest);
   free(buffer2);

   return NULL;
}
