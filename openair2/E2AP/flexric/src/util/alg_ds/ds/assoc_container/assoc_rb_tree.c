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

/*
    RB tree implemented a la CLRS by mir

    Red-Black trees have 5 properties:
    1- Every node is Red or Black
    2- The root is black
    3- Every leaf is black
    4- If a node is red, then both its children are black
    5- For each node, all simple paths from the node to descendant leaves contain the same number of black nodes 

    Lemma 1: a red-black tree with n internal nodes has a height at most 2lg(n+1)
*/

#include "assoc_rb_tree.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
  ASSOC_BLACK,
  ASSOC_RED,
} assoc_color_e;

typedef struct assoc_node_s{
  void* key;
  void* value;
  struct assoc_node_s* parent;
  struct assoc_node_s* left;
  struct assoc_node_s* right;
  assoc_color_e color;
} assoc_node_t;




static
assoc_node_t* init_node(void* key, void* value)
{
  assoc_node_t* n = malloc(sizeof(assoc_node_t));
  assert(n != NULL);
  n->key = key;
  n->value = value;
  n->parent = NULL;
  n->left = NULL;
  n->right = NULL;
  n->color = ASSOC_RED;
  return n;
}

static
assoc_node_t* create_node_rb_tree(assoc_rb_tree_t* tree, void* key, void* value)
{
  assoc_node_t* x = init_node(key,value);
  x->parent = tree->dummy;
  x->left = tree->dummy;
  x->right = tree->dummy;
  return x;
}

static
void free_node_rb_tree(assoc_node_t* n)
{
  assert(n != NULL);
  free(n->key);
  //free(n->value);
  free(n);
}

static
void depth_first_search_free(assoc_rb_tree_t* tree, assoc_node_t* node, void (*f)(void* key,void* value))
{
  assert(tree != NULL);
  assert(node != NULL);
  if(node != tree->dummy){
    depth_first_search_free(tree, node->left, f);
    depth_first_search_free(tree, node->right, f);
    if(f != NULL)
      f(node->key, node->value);
    free_node_rb_tree(node);
  }
}

void assoc_rb_tree_init(assoc_rb_tree_t* tree, size_t key_sz, int (*comp_func)(const void*, const void*), free_func_t free_func) 
{
  assert(tree != NULL);
  assert(key_sz > 0);

  tree->dummy = init_node(NULL,NULL);
  tree->dummy->color = ASSOC_BLACK;
  tree->root = tree->dummy;
  tree->comp = comp_func;
  tree->size = 0;
  tree->key_sz = key_sz;
  tree->free_func = free_func;
}

void assoc_rb_tree_free(assoc_rb_tree_t* tree)
{
  assert(tree != NULL);
  depth_first_search_free(tree, tree->root, tree->free_func);
  free(tree->dummy);
}



static
void left_rotate(assoc_rb_tree_t* tree, assoc_node_t* x_node)
{
  assert(tree != NULL);
  assert(x_node != NULL);
  assert(x_node->right != tree->dummy);
  
  assoc_node_t* y_node = x_node->right;
  x_node->right = y_node->left;
  if(y_node->left != tree->dummy)
    y_node->left->parent = x_node;

  y_node->parent = x_node->parent;
  if(x_node->parent == tree->dummy)
    tree->root = y_node;
  else if(x_node == x_node->parent->left)
    x_node->parent->left = y_node;
  else
    x_node->parent->right = y_node;

  y_node->left = x_node;
  x_node->parent = y_node;
}

static
void right_rotate(assoc_rb_tree_t* tree, assoc_node_t* x_node)
{
  assert(tree != NULL);
  assert(x_node != NULL);
  assert(x_node->left != tree->dummy);

  assoc_node_t* y_node = x_node->left;
  x_node->left = y_node->right;
  if(y_node->right != tree->dummy)
    y_node->right->parent = x_node;

  y_node->parent = x_node->parent;

  if(x_node->parent == tree->dummy)
    tree->root = y_node;
  else if( x_node->parent->left == x_node)
    x_node->parent->left = y_node;
  else
    x_node->parent->right = y_node;

  y_node->right = x_node;
  x_node->parent = y_node;

}

static
void insert_fixup(assoc_rb_tree_t* tree, assoc_node_t* z_node)
{
  while(z_node->parent->color == ASSOC_RED){
    // z_node->parent.color == RED) which means:
    // 1- violating only property 4 of the red-black tree
    // 2- z_node->parent was not the root by property 2, root must be black
    // 3- z_node->parent->parent must exist and is Black, since Red-Red is illegal

    assoc_node_t* parent = z_node->parent;
    assoc_node_t* grand_parent = z_node->parent->parent;
    if(parent == grand_parent->left){
      assoc_node_t* uncle = grand_parent->right;
      // Case 1: uncle is Red, flip colors and continue up the tree
      if(uncle->color == ASSOC_RED){
        parent->color = ASSOC_BLACK;
        uncle->color = ASSOC_BLACK;
        grand_parent->color = ASSOC_RED;
        z_node = grand_parent;
      } else { // uncle is BLACK
      // Case 2: z_node at right
        if(z_node == parent->right){
          z_node = parent;
          left_rotate(tree,z_node);
          parent = z_node->parent; // the grand_parent is the same, so do not refresh
        } 
      // Case 3: z_node at left
        parent->color = ASSOC_BLACK;
        grand_parent->color = ASSOC_RED;
        right_rotate(tree, grand_parent);
      }
    } else {
      assoc_node_t* uncle = grand_parent->left; 
      if(uncle->color == ASSOC_RED){
        parent->color = ASSOC_BLACK;
        uncle->color = ASSOC_BLACK;
        grand_parent->color = ASSOC_RED;
        z_node = grand_parent;
      } else { // uncle->color == BLACK
        if(z_node == parent->left){
          z_node = parent;
          right_rotate(tree, z_node);
          parent = z_node->parent; // the grand_parent is the same, so do not refresh
        }
        parent->color = ASSOC_BLACK;
        grand_parent->color = ASSOC_RED;
        left_rotate(tree, grand_parent);
      }
    }
  }
  // Assure that property 2 of the red-black tree is maintained
  tree->root->color = ASSOC_BLACK;
}


void assoc_rb_tree_insert(assoc_rb_tree_t* tree, void const* key, size_t key_sz, void* value)
{
  assert(tree != NULL);
  assert(tree->key_sz == key_sz && "Size mismatch");

  void* key_dst = malloc(key_sz);
  assert(key_dst != NULL && "Memory exhausted");
  memcpy(key_dst, key, key_sz);

  assoc_node_t* z_node = create_node_rb_tree(tree, key_dst, value);

  assert(z_node != tree->dummy);

  assert(z_node->parent == tree->dummy);
  assert(z_node->left == tree->dummy);
  assert(z_node->right == tree->dummy);
  assert(z_node->color == ASSOC_RED);

  assoc_node_t* x_node = tree->root;
  assoc_node_t* y_node = tree->dummy;

  // Find leaf to insert
  while(x_node != tree->dummy){
    y_node = x_node;
    if(tree->comp(z_node->key, x_node->key) == 1)
      x_node = x_node->left;
    else
      x_node = x_node->right;
  }

  z_node->parent = y_node;

  if(y_node == tree->dummy)
    tree->root = z_node;
  else if(tree->comp(z_node->key, y_node->key) == 1)
      y_node->left = z_node;
  else
    y_node->right = z_node;
  insert_fixup(tree, z_node);

  tree->size++;
}

static
void delete_fixup( assoc_rb_tree_t* tree, assoc_node_t* x_node)
{
  while(x_node != tree->root && x_node->color == ASSOC_BLACK){
    if(x_node == x_node->parent->left){
      assoc_node_t* w_node = x_node->parent->right;
      // Case 1: x's sibling is RED
      if(w_node->color == ASSOC_RED){
        w_node->color = ASSOC_BLACK;
        x_node->parent->color = ASSOC_RED;
        left_rotate(tree, x_node->parent);
        w_node = x_node->parent->right;
      }
      assert(w_node->color == ASSOC_BLACK);
      // Case 2: x's siblings nodes are both black and w itself is black
      if(w_node->left->color == ASSOC_BLACK && w_node->right->color == ASSOC_BLACK){
        w_node->color = ASSOC_RED;
        // if we come from Case 1, this will terminate, as x_node->parent == RED
        // else, spread the problem to upper levels in the tree
        // (maybe left and right branch decompensated after removing
        // black node in the left branch).
        x_node = x_node->parent;        
      } else {
      // Case 3: x's sibling w is black w->left->color == RED && w->right->color == BLACK
        if(w_node->right->color == ASSOC_BLACK){
          assert(w_node->color == ASSOC_BLACK);
          assert(w_node->left->color == ASSOC_RED);
          w_node->left->color = ASSOC_BLACK; 
          w_node->color = ASSOC_RED;
          right_rotate(tree, w_node);
          w_node = x_node->parent->right;
        }
        // Case 4: x's sibling is black and w's right child is red
        assert(w_node->color == ASSOC_BLACK);
        assert(w_node->right->color == ASSOC_RED);
        w_node->color = x_node->parent->color;
        x_node->parent->color = ASSOC_BLACK;
        // since w_node->right->color == RED we insert a new black 
        // in the w_node path, changing the color
        w_node->right->color = ASSOC_BLACK;
        // the rotation adds one black in the x_node path and deletes
        // one in the w_node path. As a result after the function, the x_node path
        // gets one extra black in the w_node, and ends with the number of blacks
        // +1, -1
        left_rotate(tree, x_node->parent); 
        x_node = tree->root;
      }
    } else {
      assoc_node_t* w_node = x_node->parent->left;
      // Case 1: x' sibling is red
      if(w_node->color == ASSOC_RED){
        w_node->color = ASSOC_BLACK;
        x_node->parent->color = ASSOC_RED;
        right_rotate(tree, x_node->parent);
        w_node = x_node->parent->left;
      }
      assert(w_node->color == ASSOC_BLACK);
      // Case 2: x' siblings nodes are both black and w itself is black
      if(w_node->left->color == ASSOC_BLACK && w_node->right->color == ASSOC_BLACK){
          w_node->color = ASSOC_RED;
          x_node = x_node->parent;
      } else {
      // Case 3: x's sibling w is black w->left == RED && w->right == BLACK
        if(w_node->left->color == ASSOC_BLACK){
          assert(w_node->color == ASSOC_BLACK);
          assert(w_node->right->color == ASSOC_RED);
          w_node->right->color = ASSOC_BLACK;
          w_node->color = ASSOC_RED;
          left_rotate(tree, w_node);
          w_node = x_node->parent->left;
        }
        // Case 4: x's sibling is black and w's left child is red
        assert(w_node->color == ASSOC_BLACK);
        assert(w_node->left->color == ASSOC_RED);
        w_node->color = x_node->parent->color;
        x_node->parent->color = ASSOC_BLACK;
        w_node->left->color = ASSOC_BLACK;
        right_rotate(tree, x_node->parent);
        x_node = tree->root;
      } 
    }
  }
  x_node->color = ASSOC_BLACK;
}

static
void transplant(assoc_rb_tree_t* tree, assoc_node_t* u_node, assoc_node_t* v_node)
{
  assert(u_node != tree->dummy);
  
  if(u_node->parent == tree->dummy)
    tree->root = v_node;
  else if( u_node == u_node->parent->left)
    u_node->parent->left = v_node;
  else
    u_node->parent->right = v_node;

  v_node->parent = u_node->parent;
}

static
assoc_node_t* minimum(assoc_rb_tree_t const* tree, assoc_node_t* x_node)
{
  if(x_node == tree->dummy)
    return tree->dummy;

  while(x_node->left != tree->dummy)
    x_node = x_node->left;
  return x_node;
}

static
assoc_node_t* find_rb_tree(assoc_rb_tree_t* tree, assoc_node_t* node, void* key)
{
  assert(tree != NULL);
  assert(node != NULL);
  assert(key != NULL);

 while(node != tree->dummy){
    int rc = tree->comp(key, node->key); 
    if(rc == 1)
      node = node->left;
    else if (rc == -1)
      node = node->right;
    else{
      assert(rc == 0);
      break;
    }
  }
  return node;
}

void* assoc_rb_tree_extract(assoc_rb_tree_t* tree, void* key)
{
  assert(tree != NULL);
  assert(key != NULL);

  assoc_node_t* z_node = find_rb_tree(tree, tree->root, key);
  assert(z_node != tree->dummy && "Trying to extract a key not found in the tree" );
  
  assoc_color_e original_color = z_node->color;
  assoc_node_t* x_node = z_node;
  assoc_node_t* y_node = z_node;
  if(z_node->left == tree->dummy){
    x_node = z_node->right;
    transplant(tree, z_node, z_node->right);
    // note that nobody points to z_node, even though z_node still points to parent and right
  } else if(z_node->right == tree->dummy){
    x_node = z_node->left;
    transplant(tree, z_node, z_node->left);
  } else {
    y_node = minimum(tree, z_node->right); 
    original_color = y_node->color;
    x_node = y_node->right;

    if(y_node->parent == z_node){
      x_node->parent = y_node; // necessary when x_node is the dummy node
    } else {
      transplant(tree, y_node, y_node->right); 
      y_node->right = z_node->right;
      y_node->right->parent = y_node; 
    }
    transplant(tree, z_node, y_node);
    y_node->left = z_node->left;
    y_node->left->parent = y_node;
    y_node->color = z_node->color;
  }
  void* value = z_node->value;
  //free(z_node->key);
  //free(z_node);

  free_node_rb_tree(z_node);
  if(original_color == ASSOC_BLACK)
    delete_fixup(tree, x_node);

  assert(tree->size != 0);
  tree->size--;
  return value;
}


void* assoc_rb_tree_front(assoc_rb_tree_t const* tree)
{
  assert(tree != NULL);
  // Returns, tree's most left child node
  assoc_node_t* node = minimum(tree, tree->root);
  return node;
}

void* assoc_rb_tree_next(assoc_rb_tree_t const* tree, void* it)
{
  assert(tree != NULL);
  assert(it != NULL);
  assoc_node_t* node = (assoc_node_t*)it;

  assert(node != tree->dummy && "No next node since the end reached");

  // a) If you have right child node find it
  if(node->right != tree->dummy){
    node = minimum(tree, node->right);
    return node;
  }

  // b) If you come from the parent's left stop. Otherwise go up untill 
  // c) you come from parent's left or
  // d) you found the end of the tree
  assoc_node_t* p = node->parent;
  while(p != tree->dummy && p->left != node){
    node = p;
    p = node->parent;
  }
  return p;
}

void* assoc_rb_tree_end(assoc_rb_tree_t const* tree)
{
  assert(tree != NULL);
  return tree->dummy;
}

void* assoc_rb_tree_key(assoc_rb_tree_t* tree, void* it)
{
  assert(tree != NULL);
  assert(it != tree->dummy && "end iterator passed?");
  return ((assoc_node_t*)it)->key;
}

// Get the value pointer form an iterator
void* assoc_rb_tree_value(assoc_rb_tree_t* tree, void* it)
{
  assert(tree != NULL);
  assert(it != tree->dummy && "end iterator passed?");
  return ((assoc_node_t*)it)->value;
}


size_t assoc_rb_tree_size(assoc_rb_tree_t* tree)
{
  assert(tree != NULL);
  return tree->size;
}

