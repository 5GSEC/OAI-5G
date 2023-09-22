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

#include "alg.h"
#include "rotate.h"

void* rotate(void* first_v, void* n_first, void* last_v, size_t elm_size)
{
  uint8_t* first = (uint8_t*)first_v; 
  uint8_t* last = (uint8_t*)last_v;  
  uint8_t* first_2 = (uint8_t*)n_first;

  do{
    iter_swap(first, first_2, elm_size);
    first += elm_size;
    first_2 += elm_size;
    if(first == n_first)
      n_first = first_2;
  } while(first_2 != last);

  void* ret = first;
  first_2 = n_first;
  
  while(first_2 != last){
    iter_swap(first, first_2,elm_size);
    first += elm_size;
    first_2 += elm_size;
    if(first == n_first)
      n_first = first_2;
    else if (first_2 == last)
      first_2 = n_first;
  }
  return ret;
}

