#include "include/rbtree.h"
#include <stdint.h>
#include <stdio.h>

/* make a big tree */
int main(int argc, char **argv) {
  struct RBTree *tree = rbtree_create();
  uint64_t v[2] = {0, 0};
  FILE *fp = fopen("/dev/random", "r");

  for (int i = 0; i < 1000000; i++) {
    if (fread(&v, 1, sizeof(v), fp) == sizeof(v)) {
      struct RBTreeNode *n = rbtree_create_node(v, v[0]);
      uintptr_t exist = 0;
      rbtree_insert(tree, n, &exist);
      if (exist != 0) {
        printf("Exist\n");
      }
    }
  }
  rbtree_dump(tree->root, 0, '#');
  rbtree_destroy(tree);
  fclose(fp);
}
