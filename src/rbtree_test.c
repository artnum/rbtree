#include "include/rbtree.h"
#include <stdint.h>
#include <stdio.h>
#if 0
void recurse(struct Index *n, struct Index *from) {
  if (n->left && n->left != from) {
    return recurse(n->left, n);
  }
  printf("KEY %05lu \n", n->key[0]);
  if (n->right && n->right != from) {
    return recurse(n->right, n);
  }
  if (n->parent == NULL) {
    return;
  }
  return recurse(n->parent, n);
}

void print_tree(struct Index *root, int deep) {
  if (!root) {
    return;
  }
  print_tree(root->left, deep + 1);
  printf("KEY %05lu %s\n", root->key[0], root->color == RED ? "RED" : "BLACK");
  print_tree(root->right, deep + 1);
}
#endif
int main(int argc, char **argv) {
  /*  uint64_t value[] = {87,  43,   32,    190,   3,      2304, 2343, 234,
                       1,   33,   58,    34533, 34,     463,  768,  98,
                       765, 7648, 78643, 6546,  245275, 72};
 */
  uint64_t value[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,
                      10, 11, 12, 13, 14, 15, 16, 17, 18};
  struct RBTree *tree = NULL;
  struct RBTreeNode *tmp = NULL;
  tree = rbtree_create();
  for (int i = 0; i < 18; i++) {
    uint64_t x[2] = {value[i], 0};
    struct RBTreeNode *new = rbtree_create_node(x, (uintptr_t)NULL);
    rbtree_insert(tree, new, NULL);
  }
  printf("--- AFTER CREATION ---\n");
  rbtree_dump(tree->root, 0, '#');
  uint64_t x[2] = {12, 0};
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
  rbtree_destroy(tree);
  return 0;
}
