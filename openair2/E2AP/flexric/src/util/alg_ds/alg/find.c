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

#include "find.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "lower_bound.h"

/*
void* find(void* start_it, void* end_it, void* value, bool (*f)(const void*, const void*))
{
  while(start_it != end_it){
    if(f(value,start_it)){
      return start_it; 
    }  

    start_it = seq_next(l,start_it);
  }
  return NULL;
}
*/


void* find_if_list(const seq_list_t* l, void* start_it, void* end_it, void* value, bool (*f)(const void*, const void*))
{
  assert(0 != 0 && "not implemented");
  assert(l != NULL);
  assert(start_it != NULL);
  assert(end_it != NULL);
  assert(value != NULL);
  assert(f != NULL);


  /*
  printf("Find sequence seq_size = %lu\n",l->size);
  while(start_it != end_it){
    if(f(value, seq_value(l,start_it))){
      printf("Value matched!!!\n");
      return start_it;
    }
    printf("Advancing iterator!\n");
    start_it = seq_next(l,start_it);
  }
  printf("Not Value matched!!!\n");
  return start_it; 
  */
}


void* find_if_arr(seq_arr_t* arr, void* start_it, void* end_it, void* value , bool(*f)(const void*, const void*))
{
  assert(arr != NULL);

  while( start_it != end_it){
    if(f(value,start_it)) 
      return start_it;
    start_it = seq_next(arr,start_it);
  }
  return start_it;
}

void* find_reg(assoc_reg_t* reg, void* start_it, void* end_it, uint32_t key)
{
  assert(reg != NULL);
  assert(start_it != NULL);
  assert(end_it != NULL);
  assert(key > 0 && "Reserved value");
  if (start_it == end_it)
    return end_it;

  void* it = lower_bound_reg(reg, start_it, end_it, key);
  assert(it <= end_it);
  if(it == end_it){ 
    return end_it;
  }

  registry_bucket_t const* b = (registry_bucket_t*)it;
  if(b->has_value == false || b->key != key){
    return end_it;
  }

  return it;
}


void* find_if_ring(seq_ring_t* arr, void* start_it, void* end_it, void* value , bool(*f)(const void*, const void*))
{
  assert(arr != NULL);
  while(start_it != end_it){
    if(f(value,start_it)) 
      return start_it;

    start_it = seq_next(arr,start_it);
  }
  return start_it;
}

void* find_if_rb_tree(assoc_rb_tree_t* tree, void* start_it, void* end_it, void const* value, bool(*f)(const void*, const void*))
{
  assert(tree != NULL);

  while(start_it != end_it){
    if(f(value, assoc_key(tree, start_it)))
      return start_it;
    start_it = assoc_next(tree, start_it);
  }

  return start_it;
}

bml_iter_t find_if_bi_map_left(bi_map_t* map, bml_iter_t start_it, bml_iter_t end_it, void const* value, bool(*f)(const void*, const void*))
{
  assert(map != NULL);
  assert(start_it.it != NULL);

  while(start_it.it != end_it.it){
    if(f(value, assoc_key(&map->left, start_it.it) ) )
      return start_it;
    start_it = bi_map_next_left(map, start_it);
  }

  return start_it;
}

bmr_iter_t find_if_bi_map_right(bi_map_t* map, bmr_iter_t start_it, bmr_iter_t end_it, void const* value, bool(*f)(const void*, const void*))
{
  assert(map != NULL);
  assert(start_it.it != NULL);

  while(start_it.it != end_it.it){
    if(f(value, start_it.it) == true)
      return start_it;

    start_it = bi_map_next_right(map, start_it);
  }

  return start_it;
}




