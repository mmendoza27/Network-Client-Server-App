#include <math.h>
#include <stdio.h>
#include <string.h>
#include "primelib.h"

int isPrimeInt(int value) {
   int currentValue = 2;
   int endWith = sqrt(value);
   
   if(value > 0 && value <= 2)
      return 1;
   
   while(currentValue <= endWith){
      if(value % currentValue == 0)
         return 0;
      
      currentValue++;
   }
   
   return 1;
}

int isPrimeLongLong(long long int value){
   long long int currentValue = 2;
   
   if(value > 0 && value <= 2)
      return 1;
   
   while((currentValue * currentValue) <= value){
      if(value % currentValue == 0)
         return 0;
      
      currentValue++;
   }
   
   return 1;
}