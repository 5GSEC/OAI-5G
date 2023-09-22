/*
MIT License

Copyright (c) 2021 Mikel Irazabal

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

#include "accumulate.h"
#include <assert.h>

uint64_t accumulate_u32(uint32_t* start, uint32_t* end)
{
  uint64_t val = 0;
  void* start_v = start;
  void* end_v = end;
  while(start_v != end_v){
    val += *(uint32_t*)start_v;
    start_v += sizeof(uint32_t); 
  }
  return val;
}

uint64_t accumulate_ring(seq_ring_t* r, void* start_it, void* end_it)
{
  assert(r != NULL);
  assert(start_it != NULL);
  assert(end_it != NULL);
  assert(r->elt_size == sizeof(uint32_t));

  uint64_t val = 0;
  void* it = start_it;
  while(it != end_it){
    val += *(uint32_t*)it;
    it = seq_next(r, it );
  }

  return val;
}

