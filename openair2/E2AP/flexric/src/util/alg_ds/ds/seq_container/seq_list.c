#include "seq_list.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static
lst_node_t* lst_node_init(void* data)
{
  lst_node_t* tmp = malloc(sizeof(lst_node_t));
  assert(tmp != NULL && "Memory exhausted while allocating a lst_node");
  tmp->next = NULL;
  tmp->data = data;
  return tmp;
}

static
void lst_node_free(lst_node_t* n)
{
  free(n);
}

void seq_list_init(seq_list_t* lst)
{
  assert(lst != NULL);
  lst->root = NULL;
  lst->size = 0; 
}

static
void seq_list_free_impl(lst_node_t* n)
{
  if(n == NULL) return;
  seq_list_free_impl(n->next);
  lst_node_free(n);
} 

void seq_list_free(seq_list_t* lst, seq_free_func fp)
{
  if(fp != NULL){
    void* start_it = seq_list_front(lst);
    void* end_it = seq_list_end(lst);
    while(start_it != end_it){
      fp(start_it);
      start_it = seq_list_next(lst, start_it);
    }
  }

  seq_list_free_impl(lst->root);    
}

void seq_list_push_back(seq_list_t* lst, void* data)
{
  assert(lst != NULL && data != NULL);

  if(lst->root == NULL){
    lst->root = lst_node_init(data);
    ++lst->size; 
    return;
  }

  lst_node_t* last = lst->root;
  while(last->next != NULL){
     last = last->next;
  }
  last->next = lst_node_init(data);
  ++lst->size; 
}

void seq_list_erase(seq_list_t* lst, void* it)
{
  assert(lst != NULL && it != NULL);
  if(lst->root == NULL) return;

  if(lst->root == (lst_node_t*)it) {
    lst->root = lst->root->next; 
    lst->size--;
    lst_node_free(it);
    return;
  }
  lst_node_t* cmp = lst->root;
  lst_node_t* next = cmp->next;
  while(next != NULL && next != (lst_node_t*)it){
    cmp = cmp->next; 
    next = next->next;
  }

  if(next == (lst_node_t*)it){
    cmp->next = next->next;
    lst->size--;
    lst_node_free(it);
    return;
  }
  assert(0!=0 && "Iterator not found in the list!");
}

void* seq_list_front(seq_list_t* lst)
{
  return lst->root;
}

void* seq_list_front_const(const seq_list_t* lst)
{
  return seq_list_front((seq_list_t*)lst);
}

void* seq_list_next(seq_list_t* lst, void* it)
{
  assert(lst != NULL && it != NULL);
  return ((lst_node_t*)it)->next;
}

void* seq_list_next_const(const seq_list_t* lst, void* it)
{
  return seq_list_next((seq_list_t*)lst, it);
}

void* seq_list_end(seq_list_t* lst)
{
  return NULL;
}

void* seq_list_end_const(const seq_list_t* lst)
{
  return seq_list_end((seq_list_t*)lst);
}

void* seq_list_at(seq_list_t* l, uint32_t pos)
{
  assert(0!=0 && "Not implemented");
  return NULL;
}

int32_t seq_list_dist(seq_list_t* l, void* first, void* last)
{
  assert(0!=0 && "Not implemented");
  return -1;
};

bool seq_list_equal(void* a, void* b)
{
  printf("Into equal function in seq list \n");
   lst_node_t* aa = (lst_node_t*)a;
   lst_node_t* bb = (lst_node_t*)b;
   if(aa == NULL || bb == NULL){
      if(aa == NULL && bb == NULL){
        printf("Both equal NULL\n");
        return true;
      }
      printf("One equal to NULL\n");
      return false;
   }
  return memcmp(a,b, sizeof(lst_node_t)) == 0 ? true : false; 
  //return (*(lst_node_t*)(a)) == (*(lst_node_t*)(b));  
}

void* seq_list_value(void* a)
{
   lst_node_t* aa = (lst_node_t*)a;
   assert(aa != NULL);
   return aa->data; 
}

size_t seq_list_size(seq_list_t* l)
{
  return l->size;
};
