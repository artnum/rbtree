#ifndef RBTREE_H__
#define RBTREE_H__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

enum RBColor { BLACK, RED };
struct RBTreeNode {
  struct RBTreeNode *parent;
  union {
    struct {
      struct RBTreeNode *left;
      struct RBTreeNode *right;
    };
    struct RBTreeNode *child[2];
  };
  enum RBColor color;
  uint64_t key[2]; /* we store indexes that can be uuid */
  uintptr_t data;
};

struct RBTree {
  struct RBTreeNode *root;
};

struct RBTree *rbtree_create();
void rbtree_insert(struct RBTree *tree, struct RBTreeNode *idx,
                   uintptr_t *oldata);
uintptr_t rbtree_search(struct RBTree *tree, uint64_t *key);
struct RBTreeNode *rbtree_delete(struct RBTree *tree, uint64_t key[2]);
struct RBTreeNode *rbtree_create_node(uint64_t key[2], uintptr_t data);
void rbtree_dump(struct RBTreeNode *root, int level, char branch);
void rbtree_free_node(struct RBTreeNode *idx);
#endif /* RBTREE_H__ */
