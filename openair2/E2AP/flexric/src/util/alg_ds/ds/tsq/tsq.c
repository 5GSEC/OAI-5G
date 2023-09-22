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

#include "tsq.h"


void init_tsq(tsq_t* q, size_t elm_sz)
{
  assert(q != NULL);
  assert(elm_sz > 0);

  seq_init(&q->r, elm_sz);

  pthread_mutexattr_t *mtx_attr = NULL;
#ifdef DEBUG
  *mtx_attr = PTHREAD_MUTEX_ERRORCHECK;
#endif

  int rc = pthread_mutex_init(&q->mtx, mtx_attr );
  assert(rc == 0);
}

void free_tsq(tsq_t* q, void (*f)(void*) )
{
  assert(q != NULL);

  seq_free(&q->r, f);

  int rc = pthread_mutex_destroy(&q->mtx);
  assert(rc == 0); 
}

void push_tsq(tsq_t* q, void* val, size_t sz)
{
  assert(q != NULL);
  assert(val != NULL);
  assert(sz > 0);

  lock_guard(&q->mtx);
  seq_push_back(&q->r, val, sz);
}

void pop_tsq(tsq_t* q, void* val, size_t elm_sz)
{
  assert(q != NULL);
  assert(q->r.elt_size == elm_sz);

  lock_guard(&q->mtx);
  assert(seq_size(&q->r) != 0 && "trying to pop from a void queue" );

  void* it = seq_ring_front(&q->r);

  memcpy(val, it, elm_sz);

  assert(it != seq_end(&q->r));
  void* next = seq_next(&q->r, it);
   
  seq_erase(&q->r, it, next);
}

size_t size_tsq(tsq_t* q)
{
  assert(q != NULL);
  
  lock_guard(&q->mtx);
  return seq_size(&q->r);
}

