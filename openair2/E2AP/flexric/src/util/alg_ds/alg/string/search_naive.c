#include "search_naive.h"

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

char* search_naive(size_t len_needle, char needle[len_needle], size_t len_haystack, char haystack[len_haystack])
{
  assert(len_needle != 0);
  assert(len_haystack != 0);
  assert(len_haystack >= len_needle);

  for(size_t i = 0; i < len_haystack; ++i){
    size_t j = 0;
    size_t k = i;
    for(; j < len_needle; ++j){
      if(haystack[k] != needle[j])
        break;
      ++k;
    }
    if(j == len_needle)
      return &haystack[i];
  }
  return NULL;
}

