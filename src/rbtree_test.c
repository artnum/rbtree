#include "include/rbtree.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  uint64_t value[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,
                      10, 11, 12, 13, 14, 15, 16, 17, 18};

  struct RBTree *tree = NULL;
  struct RBTreeNode *tmp = NULL;
  tree = rbtree_create();
  for (int i = 0; i < 18; i++) {
    uint64_t x[2] = {value[i], 0};
    struct RBTreeNode *new = rbtree_create_node(x, (uintptr_t)x[0]);
    rbtree_insert(tree, new, NULL);
  }

  printf("--- AFTER CREATION ---\n");
  rbtree_dump(tree->root, 0, '#');
  printf("LEFT MOST %lu / RIGHT MOST %lu\n", rbtree_leftmost(tree),
         rbtree_rightmost(tree));

  /* change key with id 13 and set id to 20, value 13 must become the right most
   * value */
  uint64_t y[2] = {20, 0};
  uint64_t x[2] = {13, 0};
  rbtree_change_key(tree, x, y);
  printf("--- AFTER KEY CHANGE ---\n");
  rbtree_dump(tree->root, 0, '#');

  printf("LEFT MOST %lu / RIGHT MOST %lu\n", rbtree_leftmost(tree),
         rbtree_rightmost(tree));

  /* delete some node and then print the tree */
  x[0] = 12;
  tmp = rbtree_delete(tree, x);
  if (tmp) {
    rbtree_free_node(tmp);
  }

  x[0] = 2;
  tmp = rbtree_delete(tree, x);
  if (tmp) {
    rbtree_free_node(tmp);
    tmp = NULL;
  }
  x[0] = 8;
  tmp = rbtree_delete(tree, x);
  if (tmp) {
    rbtree_free_node(tmp);
    tmp = NULL;
  }
  x[0] = 15;
  tmp = rbtree_delete(tree, x);
  if (tmp) {
    rbtree_free_node(tmp);
    tmp = NULL;
  }

  printf("--- AFTER DELETION ---\n");
  rbtree_dump(tree->root, 0, '#');

  /* destroy the tree with rbtree_function */
  rbtree_destroy(tree);

  /* Create a new tree and destroy id "manually" */
  tree = rbtree_create();
  for (int i = 0; i < 18; i++) {
    uint64_t x[2] = {value[i], 0};
    struct RBTreeNode *new = rbtree_create_node(x, (uintptr_t)NULL);
    rbtree_insert(tree, new, NULL);
  }
  for (int i = 0; i < 18; i++) {
    uint64_t x[2] = {value[i], 0};
    struct RBTreeNode *old = rbtree_delete(tree, x);
    rbtree_free_node(old);
  }
  free(tree);

  return 0;
}
