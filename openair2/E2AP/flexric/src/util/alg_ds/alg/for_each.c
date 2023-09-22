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

#include "for_each.h"
#include <assert.h>

void for_each_arr(seq_arr_t* arr, void* start_it, void* end_it, void(*f)(const void*, const void*), void* data)
{
  assert(arr != NULL);
  assert(start_it != NULL);
  assert(end_it != NULL);
  for( ; start_it != end_it; start_it += arr->elt_size){
    f(start_it,data);
  }
}


static inline
size_t bucket_sz(assoc_reg_t* reg)
{
  return sizeof(registry_bucket_t) + sizeof( uint8_t[ reg->elm_sz] );
}


void for_each_reg(assoc_reg_t* reg, void* start_it, void* end_it, void(*f)(const void*, const void*), void* data)
{
  assert(reg != NULL);
  assert(start_it != NULL);

  size_t const b_size = bucket_sz(reg);  
  assert( (start_it - reg->arr)/b_size < reg->cap && "Iterator out of the range"); 
  assert(end_it != NULL);
 
  for( ; start_it != end_it; start_it += b_size){
    registry_bucket_t const* b = start_it;
    if(b->has_value)
      f(start_it,data);
  }
}

