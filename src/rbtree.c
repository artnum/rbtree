#include "include/rbtree.h"
#include <assert.h>
#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* red-black tree based on postgresql implementation
 * https://doxygen.postgresql.org/rbtree_8c_source.html
 */

/* use to have a sizeof available without doint sizeof(((struct RBTreeNode
 * *)NULL)->key) which is UB even if linux kernel has
 * # define offsetof(TYPE, MEMBER)	((size_t)&((TYPE *)0)->MEMBER)
 */
static struct RBTreeNode __node;
int cmp_key(const RBTREE_KEY_BASE_TYPE *a, const RBTREE_KEY_BASE_TYPE *b) {
  const size_t size = sizeof(__node.key) / sizeof(*a);
  for (size_t i = 0; i < size; i++) {
    if (a[i] < b[i]) {
      return -1;
    }
    if (a[i] > b[i]) {
      return 1;
    }
  }
  return 0;
}

#define IS_NIL(n) ((n) == (&__sentinel))
#define SENTINEL (&__sentinel)
#define IS_BLACK(n) ((n)->color == BLACK)
#define IS_RED(n) (!IS_BLACK(n))
#define SET_BLACK(n) ((n)->color = BLACK)
#define SET_RED(n) ((n)->color = RED)
enum RBDirection { LEFT, RIGHT };

static struct RBTreeNode __sentinel = {
    .color = BLACK, .left = SENTINEL, .right = SENTINEL, .parent = NULL};

void rbtree_free_node(struct RBTreeNode *idx) { free(idx); }

void rbtree_dump(struct RBTreeNode *root, int level, char branch) {
  if (IS_NIL(root)) {
    return;
  }

  for (int i = 0; i < level; i++) {
    printf("  ");
  }
#if __BYTE_ORDER == __LITTLE_ENDIAN
  for (int i = sizeof(root->key) - 1; i >= 0; i--) {
#else
  for (int i = 0; i < sizeof(root->key); i++) {
#endif
    printf("%x", ((uint8_t *)root->key)[i]);
  }
  printf(" -- %c (%s)\n", branch, IS_RED(root) ? "RED" : "BLACK");
  rbtree_dump(root->left, level + 1, 'L');
  rbtree_dump(root->right, level + 1, 'R');
}

struct RBTreeNode *_rbtree_search(struct RBTree *tree,
                                  const RBTREE_KEY_BASE_TYPE *key,
                                  struct RBTreeNode **parent) {
  assert(tree != NULL);
  assert(key != NULL);

  if (parent) {
    *parent = NULL;
  }
  if (tree->root == NULL) {
    return SENTINEL;
  }

  struct RBTreeNode *node = tree->root;
  for (;;) {
    if (IS_NIL(node)) {
      return SENTINEL;
    }
    int cmp = cmp_key(key, node->key);
    if (cmp == 0) {
      return node;
    } else if (cmp < 0) {
      if (parent) {
        *parent = node;
      }
      node = node->left;
    } else {
      if (parent) {
        *parent = node;
      }
      node = node->right;
    }
  }
}

inline static void _rotate(struct RBTree *tree, struct RBTreeNode *x,
                           enum RBDirection direction) {
  struct RBTreeNode *y = x->child[!direction];
  x->child[!direction] = y->child[direction];
  if (!IS_NIL(y->child[direction])) {
    y->child[direction]->parent = x;
  }

  if (!IS_NIL(y)) {
    y->parent = x->parent;
  }
  if (x->parent) {
    x->parent->child[x == x->parent->right] = y;
  } else {
    tree->root = y;
  }
  y->child[direction] = x;
  if (!IS_NIL(x)) {
    x->parent = y;
  }
}

inline static void _insert_fixup(struct RBTree *tree, struct RBTreeNode *x) {
  while (x != tree->root && IS_RED(x->parent)) {
    enum RBDirection direction = x->parent == x->parent->parent->right;
    struct RBTreeNode *y = x->parent->parent->child[!direction];
    if (IS_RED(y)) {
      SET_BLACK(x->parent);
      SET_BLACK(y);
      SET_RED(x->parent->parent);

      x = x->parent->parent;
    } else {
      if (x == x->parent->child[!direction]) {
        x = x->parent;
        _rotate(tree, x, direction);
      }
      SET_BLACK(x->parent);
      SET_RED(x->parent->parent);
      _rotate(tree, x->parent->parent, !direction);
    }
  }
  SET_BLACK(tree->root);
}
void rbtree_insert(struct RBTree *tree, struct RBTreeNode *node,
                   uintptr_t *olddata) {
  struct RBTreeNode *parent = NULL;
  struct RBTreeNode *found = _rbtree_search(tree, node->key, &parent);

  if (IS_NIL(found) && parent == NULL) {
    tree->root = node;
    SET_BLACK(tree->root);
    return;
  }

  if (IS_NIL(found)) {
    SET_RED(node);
    node->parent = parent;
    if (cmp_key(node->key, parent->key) < 0) {
      parent->left = node;
    } else {
      parent->right = node;
    }
    _insert_fixup(tree, node);
  } else {
    if (olddata) {
      *olddata = found->data;
    }
    found->data = node->data;
    rbtree_free_node(node);
  }
}

struct RBTree *rbtree_create() {
  struct RBTree *tree = calloc(1, sizeof(struct RBTree));
  return tree;
}
void rbtree_destroy(struct RBTree *tree) {
  struct RBTreeNode *node = NULL;
  node = tree->root;
  if (!node) {
    free(tree);
    return;
  }

  do {
    while (!IS_NIL(node->left)) {
      node = node->left;
    }
    if (!IS_NIL(node->right)) {
      node = node->right;
      continue;
    }
    struct RBTreeNode *c = node;
    c = node;
    node = node->parent;
    if (node) {
      node->child[c == node->right] = SENTINEL;
    }
    rbtree_free_node(c);
  } while (!IS_NIL(node) && node != NULL);

  free(tree);
}

struct RBTreeNode *rbtree_create_node(RBTREE_KEY(key), uintptr_t data) {
  struct RBTreeNode *new = calloc(1, sizeof(*new));
  if (new) {
    memcpy(new->key, key, sizeof(new->key));
    new->data = data;
    new->left = SENTINEL;
    new->right = SENTINEL;
    new->parent = NULL;
    SET_RED(new);
  }

  return new;
}

uintptr_t rbtree_search(struct RBTree *tree, const RBTREE_KEY_BASE_TYPE *key) {
  struct RBTreeNode *node = _rbtree_search(tree, key, NULL);
  if (IS_NIL(node)) {
    return (uintptr_t)NULL;
  }
  return node->data;
}

inline static void _overwrite_node(struct RBTreeNode *a, struct RBTreeNode *b) {
  memcpy(a->key, b->key, sizeof(a->key));
  a->data = b->data;
}

inline static void _delete_fixup(struct RBTree *tree, struct RBTreeNode *x) {
  while (x != tree->root && IS_BLACK(x)) {
    enum RBDirection direction = x == x->parent->right;
    struct RBTreeNode *w = x->parent->child[!direction];
    if (IS_RED(w)) {
      SET_BLACK(w);
      SET_RED(x->parent);
      _rotate(tree, x->parent, direction);
      w = x->parent->child[!direction];
    }
    if (IS_BLACK(w->child[direction]) && IS_BLACK(w->child[!direction])) {
      SET_RED(w);
      x = x->parent;
    } else {
      if (IS_BLACK(w->child[!direction])) {
        SET_BLACK(w->child[!direction]);
        SET_RED(w);
        _rotate(tree, w, !direction);
      }
      SET_BLACK(w->left);
      w->color = x->parent->color;
      SET_BLACK(x->parent);
      _rotate(tree, x->parent, direction);
      x = tree->root;
    }
  }
  SET_BLACK(x);
}

struct RBTreeNode *rbtree_delete(struct RBTree *tree, RBTREE_KEY(key)) {
  struct RBTreeNode *z = _rbtree_search(tree, key, NULL);
  if (!z || IS_NIL(z)) {
    return NULL;
  }
  struct RBTreeNode *y, *x;

  /* any children */
  if (IS_NIL(z->left) || IS_NIL(z->right)) {
    y = z;
  } else {
    /* two children, get successor */
    y = z->right;
    while (!IS_NIL(y->left)) {
      y = y->left;
    }
    /* overwrite target node with successor */
    _overwrite_node(z, y);
  }

  /* from here, we deal only with single or no child node */
  x = y->child[!IS_NIL(y->right)];

  /* disconnect node */
  x->parent = y->parent;
  if (y->parent) {
    y->parent->child[y == y->parent->right] = x;
  } else {
    tree->root = x;
  }

  if (IS_BLACK(y)) {
    _delete_fixup(tree, x);
  }

  y->parent = NULL;
  memset(y->child, 0, sizeof(y->child));
  return y;
}

uintptr_t rbtree_leftmost(struct RBTree *tree) {
  assert(tree != NULL);
  if (!tree->root) {
    return (uintptr_t)NULL;
  }
  struct RBTreeNode *n = tree->root;
  while (!IS_NIL(n->left)) {
    n = n->left;
  }
  return n->data;
}

uintptr_t rbtree_rightmost(struct RBTree *tree) {
  assert(tree != NULL);
  if (!tree->root) {
    return (uintptr_t)NULL;
  }
  struct RBTreeNode *n = tree->root;
  while (!IS_NIL(n->right)) {
    n = n->right;
  }
  return n->data;
}

void rbtree_change_key(struct RBTree *tree, RBTREE_KEY(old_key),
                       RBTREE_KEY(new_key)) {
  struct RBTreeNode *n = rbtree_delete(tree, old_key);
  if (n != NULL && !IS_NIL(n)) {
    memcpy(n->key, new_key, sizeof(n->key));
    n->left = SENTINEL;
    n->right = SENTINEL;
    rbtree_insert(tree, n, NULL);
  }
}
