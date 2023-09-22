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
#include <unistd.h>

#include "../lock_guard/lock_guard.h"
#include "tsn_queue.h"

void init_tsnq(tsnq_t* q, size_t elm_sz)
{
  assert(q != NULL);
  assert(elm_sz > 0);

  seq_init(&q->r, elm_sz);

  const pthread_condattr_t* cond_attr = NULL;
  int rc = pthread_cond_init(&q->cv, cond_attr);
  assert(rc == 0);

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK;
#endif

  rc = pthread_mutex_init(&q->mtx, mtx_attr );
  assert(rc == 0);

  q->stop_token = false;
  q->stopped = false;
}

void free_tsnq(tsnq_t* q, void (*f)(void*) )
{
  assert(q != NULL);

  q->stop_token = true;

 {
  lock_guard(&q->mtx);
  pthread_cond_signal(&q->cv);
 }

  while(q->stopped == false)
    usleep(1000);

  seq_free(&q->r, f);

  int rc = pthread_cond_destroy(&q->cv);
  assert(rc == 0); 

  rc = pthread_mutex_destroy(&q->mtx);
  assert(rc == 0); 
}

void push_tsnq(tsnq_t* q, void* val, size_t sz)
{
  lock_guard(&q->mtx);

  seq_push_back(&q->r, val, sz);
  pthread_cond_signal(&q->cv);
}

void* wait_and_pop_tsnq(tsnq_t* q, void* (*f)(void*) )
{
  assert(q != NULL);

  pthread_mutex_lock(&q->mtx);

  int rc = 0;
  while(seq_size(&q->r) == 0 && rc == 0 && q->stop_token == false) {
    rc = pthread_cond_wait(&q->cv, &q->mtx);
  }

  if(q->stop_token == true){
    pthread_mutex_unlock(&q->mtx);
    q->stopped = true;
    return NULL;
  }

  assert(rc == 0);

  void* it = seq_ring_front(&q->r);

  void* elm = f(it);

  assert(it != seq_end(&q->r));
  void* next = seq_next(&q->r, it);
   
  seq_erase(&q->r, it, next);

  pthread_mutex_unlock(&q->mtx);

  return elm;
}

void* pop_tsnq_10(tsnq_t* q, void* (*f)(void*) )
{
  assert(q != NULL);

  pthread_mutex_lock(&q->mtx);

  if(q->stop_token == true){
    pthread_mutex_unlock(&q->mtx);
    q->stopped = true;
    return NULL;
  }

  assert(seq_size(&q->r) >= 10);

  void* it = seq_ring_front(&q->r);

  void* elm = NULL;
  
  assert(it != seq_end(&q->r));
  void* next = it;
  for(int i = 0; i < 10; ++i){
    elm = f(next);  
    next = seq_next(&q->r, next);
  } 
  seq_erase(&q->r, it, next);

  pthread_mutex_unlock(&q->mtx);

  return elm;
}

void* pop_tsnq_100(tsnq_t* q, void* (*f)(void*) )
{
  assert(q != NULL);

  pthread_mutex_lock(&q->mtx);

  if(q->stop_token == true){
    pthread_mutex_unlock(&q->mtx);
    q->stopped = true;
    return NULL;
  }

  assert(seq_size(&q->r) >= 100);

  void* it = seq_ring_front(&q->r);

  void* elm = NULL;

  assert(it != seq_end(&q->r));
  void* next = it;
  for(int i = 0; i < 100; ++i){
    elm = f(next);  
    next = seq_next(&q->r, next);
  }

  seq_erase(&q->r, it, next);

  pthread_mutex_unlock(&q->mtx);

  return elm;
}

size_t size_tsnq(tsnq_t* q)
{
  assert(q != NULL);
  
  lock_guard(&q->mtx);
  return seq_size(&q->r);
}

