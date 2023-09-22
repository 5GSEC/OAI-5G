/*
MIT License

Copyright (c) 2022 Mikel Irazabal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "tsn_queue.h"


typedef struct
{
  bool has_value;
  uint8_t val[1024];
  uint32_t n;
} val_t;

static
pthread_t t;

static
void* dereference(void* it)
{
  if(it == NULL)
    return NULL;

  val_t* v = malloc(sizeof(val_t)); 
  memcpy(v,it,sizeof(val_t));
  return v;
}

static
void* worker_thread(void* arg)
{
  tsq_t* q = (tsq_t*)arg;
  while(true){
   val_t* v = wait_and_pop_tsq(q, dereference); 
    if(v == NULL) break;

    assert(v != NULL);
    free(v);
  } 

  q->stopped = true;

  return NULL;
}

static
void free_value(void* it)
{
  assert(it != NULL);
  val_t* v = *(val_t**)it; 
  free(v);
}

int main()
{
  tsq_t q = {0};
  init_tsq(&q, sizeof(val_t));

  int rc = pthread_create(&t, NULL, worker_thread, &q);
  assert(rc == 0);

  int n = 0;
  for(size_t i = 0; i < 8192; ++i){
    val_t v = {0};
    v.n = n;
    push_tsq(&q, &v, sizeof(val_t) );
    ++n;
  }


  free_tsq(&q, free_value );
  pthread_join(t, NULL);

  return 0;
}

