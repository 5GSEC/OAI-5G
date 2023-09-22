
#include "assoc_reg.h"
#include "../../alg/find.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct{
  int64_t i;
  int32_t j;
  int16_t k;
  int8_t l;
} dummy_t;

void push_back_test()
{
  assoc_reg_t reg;
  assoc_reg_init(&reg, sizeof(dummy_t)); 

  for(int i =0; i < 512; ++i){
    dummy_t d = {.i = i + 100 };
    assoc_reg_push_back(&reg,&d, sizeof(dummy_t));
    test_invariants_registry_hold(&reg);
  }

 assoc_reg_free(&reg);
  //free_registry(&reg);
}

void find_registry_test()
{
  assoc_reg_t reg;
  assoc_reg_init(&reg, sizeof(dummy_t)); 

  uint32_t keys[512];
  for(int i =0; i < 512; ++i){
    dummy_t d = {.i = i + 100 };
    keys[i] = assoc_reg_push_back(&reg,&d, sizeof(dummy_t));
    test_invariants_registry_hold(&reg);
  }
  for(int i = 0; i < 512; ++i){
    void* start_it =  assoc_reg_front(&reg);
    void* end_it = assoc_reg_end(&reg);
    assert(start_it != end_it);

    void* it = find_reg(&reg, start_it, end_it, &keys[i]);
    assert(it != NULL);
    void* next_it = assoc_reg_next(&reg, it);
    assert(it != NULL);

    assoc_reg_erase(&reg, it, next_it);

//    delete_registry(&reg, keys[i]);
    test_invariants_registry_hold(&reg);
  }

  for(int i = 0; i < 512; ++i){
    void* start_it = assoc_reg_front(&reg);
    void* end_it = assoc_reg_end(&reg);
    void* it = find_reg(&reg, start_it, end_it, &keys[i]);
    assert(it == end_it);
  }
 // free_registry(&reg);
 assoc_reg_free(&reg);
}

void free_randomly_test()
{
  const int val = 4096;
  assoc_reg_t reg = {0};
  assoc_reg_init(&reg, sizeof(dummy_t)); 

  for(int i =0; i < val; ++i){
    dummy_t d = {.i = i + 100 };
    assoc_reg_push_back(&reg, &d, sizeof(dummy_t) );
    test_invariants_registry_hold(&reg);
  }

  time_t t;
  srand((unsigned) time(&t));

  for(int i = 0; i < val*4; ++i){
    uint32_t rand_num =rand() % val;
    void* start_it =  assoc_reg_front(&reg);
    void* end_it = assoc_reg_end(&reg);
    void* it = find_reg(&reg, start_it, end_it, &rand_num);
    if(it != end_it){
      void* next_it = assoc_reg_next(&reg, it);
      assoc_reg_erase(&reg, it, next_it);
    }

    test_invariants_registry_hold(&reg);
  }
  printf("Not deleted items = %ld \n", reg.sz);

 assoc_reg_free(&reg);
}

void free_randomly_test_2()
{
  const int val = 5000;
  assoc_reg_t reg = {0};
  assoc_reg_init(&reg, sizeof(dummy_t)); 

  for(int i =0; i < val; ++i){
    dummy_t d = {.i = i + 100 };

    assoc_reg_push_back(&reg, &d, sizeof(dummy_t) );
    //push_back_registry(&reg,&d, sizeof(dummy_t));
    test_invariants_registry_hold(&reg);
  }

  for(int i = 0; i < val*4; ++i){
    uint32_t rand_num =rand() % val;
    void* start_it =  assoc_reg_front(&reg);
    void* end_it = assoc_reg_end(&reg);

    void* it = find_reg(&reg, start_it, end_it, &rand_num);
    if(it != end_it){
      void* next_it = assoc_reg_next(&reg, it);
      //assert(next_it != NULL);
      assoc_reg_erase(&reg, it, next_it);
    }

    test_invariants_registry_hold(&reg);
  }

  printf("Not deleted items in test_2 = %ld \n", reg.sz);

  for(int i =0; i < val*5; ++i){
    dummy_t d = {.i = i + 100 };

    assoc_reg_push_back(&reg, &d, sizeof(dummy_t) );
    test_invariants_registry_hold(&reg);
    if(i%2 == 0){
      uint32_t rand_num = rand() % (5*val);
      // void* it = find_registry(&reg, rand_num );
      void* start_it =  assoc_reg_front(&reg);
      void* end_it = assoc_reg_end(&reg);

      void* it = find_reg(&reg, start_it, end_it, &rand_num);
      if(it != end_it){
        void* next_it = assoc_reg_next(&reg, it);
        assoc_reg_erase(&reg, it, next_it);
      }
      test_invariants_registry_hold(&reg);
    }
  }

  printf("Not deleted items in test_2 = %ld \n", reg.sz);


 assoc_reg_free(&reg);
//  free_registry(&reg);
}



int main()
{
  push_back_test();
  find_registry_test();
  free_randomly_test();
  free_randomly_test_2();
  return 0;
}

