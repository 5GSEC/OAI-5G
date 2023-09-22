#include "lock_guard.h"

#include <stdio.h>

static
int dummy_func(pthread_mutex_t* m)
{
  lock_guard(m);
   for(int i =0; i < 10; ++i){
    printf("Value of the function = %d \n",i);
  }
   return 0;
}


int main()
{
  pthread_mutex_t mtx; // = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutexattr_t attr; 

  int rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  assert(rc == 0);

  rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);


  dummy_func(&mtx);

  {
    lock_guard(&mtx);
  }

  lock_guard(&mtx);

//  rc = pthread_mutex_unlock(&mtx);
//  if(rc != 0){
//    fprintf(stdout, "Error while locking: %s\n", strerror(rc) ); 
//    exit(-1);
//   } 


  for(int i =0; i < 10; ++i){
    printf("Value of i = %d \n",i);
  }

  return 0;
}

