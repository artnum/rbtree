#ifndef RBTREE_H__
#define RBTREE_H__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* original code I used need to store 128 bits key. So I have uint64_t key[2],
 * but by redefining this, any one can change the key size at compile
 */
#if !defined RBTREE_KEY && !defined RBTREE_KEY_BASE_TYPE
#define RBTREE_KEY(name) uint64_t name[2]
#define RBTREE_KEY_BASE_TYPE uint64_t
#endif /* !defined RBTREE_KEY && !defined RBTREE_KEY_BASE_TYPE */

enum RBColor { BLACK, RED };
struct RBTreeNode {
  struct RBTreeNode *parent;

  union { // allow to use either ->child or ->left,->right
    struct {
      struct RBTreeNode *left;
      struct RBTreeNode *right;
    };
    struct RBTreeNode *child[2];
  };

  enum RBColor color;
  RBTREE_KEY(key); /* we store indexes that can be uuid */
  uintptr_t data;
};

struct RBTree {
  struct RBTreeNode *root;
};

/**
 * Create a Red Black tree.
 */
struct RBTree *rbtree_create();

/**
 * Create a Red Black tree node
 */
struct RBTreeNode *rbtree_create_node(RBTREE_KEY(key), uintptr_t data);

/**
 * Insert the node in the tree
 *
 * @param oldata The data being inserted are replaced if the node is already
 * present, if olddata is not NULL, the previous data are set here. In any way,
 * new  data are set and the node is freed if needed
 *
 * @note Insertion doesn't do any memory allocation, so it cannot fail
 */
void rbtree_insert(struct RBTree *tree, struct RBTreeNode *idx,
                   uintptr_t *oldata);

/**
 * Search for a key, return the data
 */
uintptr_t rbtree_search(struct RBTree *tree, const RBTREE_KEY_BASE_TYPE *key);

/**
 * Remove the node from the tree. The node is not freed
 */
struct RBTreeNode *rbtree_delete(struct RBTree *tree, RBTREE_KEY(key));

/**
 * Free a node created by rbtree_create_node
 */
void rbtree_free_node(struct RBTreeNode *idx);

/**
 * Destroy the whole tree
 */
void rbtree_destroy(struct RBTree *tree);

/**
 * Dump the tree to stdio
 */
void rbtree_dump(struct RBTreeNode *root, int level, char branch);

/**
 * Get left most value
 */
uintptr_t rbtree_leftmost(struct RBTree *tree);

/**
 * Get right most value
 */
uintptr_t rbtree_rightmost(struct RBTree *tree);

/**
 * Change a key
 */
void rbtree_change_key(struct RBTree *tree, RBTREE_KEY(old_key),
                       RBTREE_KEY(new_key));
#endif /* RBTREE_H__ */
