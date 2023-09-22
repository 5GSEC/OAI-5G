#include "search_naive.h"
#include "booyer_moore.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>

int main()
{

  char* haystack = "asddderffrtgasadastrtyutiwejowjdasdaallimmmasdsseeaasdaaassswwweertyyyukkkiiakakkasdfkkmmmniinbifijaodsjaiosjdjiljnnaisdhnajidsalibmaclibq";

  time_t t;
   /* Intializes random number generator */
   srand((unsigned) time(&t));

  int val = rand() % strlen(haystack) - 2;
  int len_needle = (rand() % (strlen(haystack) - val -1)) + 1;

  char* needle = haystack + val;

//  char* needle = "naisdhnaj";

  char* it_naive = search_naive(len_needle, needle, strlen(haystack), haystack);

  assert(it_naive != NULL);

  const char* it_booyer_moore = search_booyer_moore(len_needle, needle, strlen(haystack), haystack);

  assert(it_naive == it_booyer_moore);
  printf("Both algorithms returned the same pointer! \n %s \n %s \n for needle %s with size = %d\n", it_naive, it_booyer_moore, needle, len_needle);

  return 0;
}

