#include "booyer_moore.h"
#include <assert.h>
#include <stdlib.h>


#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
   _a > _b ? _a : _b; })


// Rigthmost occurrence of character
// Known as Bad character Heuristic
//
//


typedef struct
{
  int arr[256]; 
} delta_1_t;

typedef struct
{
  int* shift;
  size_t len;
} delta_2_t;

// Known as Bad character Heuristics
static inline
delta_1_t pre_process_delta_1(size_t len, char needle[len])
{
  delta_1_t d = {0};
  for(int i = 0; i < 256; ++i)
    d.arr[i] = -1;


  for(size_t i = 0; i < len; ++i){
    d.arr[(int)needle[i]] = i; 
  }

  return d;
}


// Known as Good suffix Heuristic
static inline
delta_2_t pre_process_delta_2(size_t len, char needle[len])
{
  size_t i = len;
  size_t j = len + 1;

  delta_2_t d = {.len = j, .shift = calloc(j, sizeof(int)) };
  assert(d.shift != NULL);

  int* pattern = calloc(j, sizeof(int)); 
  assert(pattern != NULL);

  pattern[i] = j;
  while(i > 0){ 
    while(j < len + 1  && needle[i-1] != needle[j-1]){
      if (d.shift[j] == 0)
        d.shift[j] = j-i;
      j = pattern[j];
    }
    i--; 
    j--;
    pattern[i] = j;
  }

// Case 2
  j = pattern[0]; 
  for(size_t i = 0; i < len +1; ++i){
    if(d.shift[i] == 0)
      d.shift[i] = j;
    if(j == i)
      j = pattern[j];
  }
  return d;
}

char* search_booyer_moore(size_t len_needle, char needle[len_needle], size_t len_haystack, char haystack[len_haystack])
{
  if(len_needle == 0 || len_haystack == 0)
    return NULL;

  delta_1_t d1 = pre_process_delta_1(len_needle, needle); 
  delta_2_t d2 = pre_process_delta_2(len_needle, needle); 

  size_t i = len_needle - 1;
  size_t j = i;

  while(i < len_haystack){
    if(haystack[i] != needle[j]){
      const size_t shift1 = len_needle - 1 - d1.arr[ (size_t)haystack[i]]; // find next occurence of the missed character 
      const size_t shift2 = d2.shift[j+1]; // Look if the pattern found so far is repeated within the string
      const size_t min_val = 1;
      const size_t shift = max(min_val, max(shift1, shift2)); 
      i += shift;
      j = len_needle - 1;
    } else {

    if(j == 0) 
      return &haystack[i];

    --j;
    --i;
    }
  }
  return NULL;
}



