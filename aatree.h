/*
** pem 2018-10-15
**
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* More than we need really, 16 bits should be enough, maybe even 8.
   In theory, the number of bits needed is log2(h) where h is the
   height of the tree. */
typedef uint32_t aatree_level_t;
#define AATREE_MAX_DEPTH 64

typedef struct aatree_node_s aatree_node_t;

struct aatree_node_s
{
    aatree_node_t *left, *right;
    aatree_level_t level;
};

typedef struct aatree_s aatree_t;

typedef int aatree_compare_fun_t(aatree_t *, void *keyp, aatree_node_t *);
typedef void aatree_swap_fun_t(aatree_t *, aatree_node_t *, aatree_node_t *);
typedef bool aatree_condition_fun_t(aatree_t *, aatree_node_t *);

struct aatree_s
{
    aatree_node_t *root;
    aatree_compare_fun_t *compare;
    aatree_swap_fun_t *swap;
};

typedef struct aatree_iter_s
{
    void *keyp;
    aatree_t *base;
    uint32_t i;
    aatree_node_t *node[AATREE_MAX_DEPTH];
} aatree_iter_t;

/* Initialize a new node */
void aatree_init_node(aatree_node_t *n);

/* Insert the node into the tree. */
void aatree_insert_node(aatree_t *t, void *keyp, aatree_node_t *n);

/* Insert the node into the tree. The key must be unique. If not,
   no insertion is done  the already existing node is returned. If inserted,
   NULL is returned. */
aatree_node_t *aatree_insert_unique_node(aatree_t *t,
                                         void *keyp, aatree_node_t *n);

/* Insert the node into the tree. If a node with the same key already
   exists, the value of that node is replaced and QQQ 
   Note that when replaced, the node 'n' is unused, only its value
   is copied. 
   QQQ Returns the new tree root. */
aatree_node_t *aatree_replace_node(aatree_t *t,
                                   void *keyp, aatree_node_t *n);

/* Remove a node with matching key. If 'cond' is given, the condition
   must return true as well for it to match. *nodep is set to the removed
   node if it was found, or NULL otherwise. It will remove the first matching
   node it encounters in the tree.
   Returns the new tree root. */
aatree_node_t *aatree_remove_node(aatree_t *t, void *keyp,
                                  aatree_condition_fun_t *cond);

/* Find a node matching 'key'. If a 'cond' is provided, this is called
   and must return true for it to be a match.
   It will return the first matching node it encounters in the tree.
   Returns the found node, or NULL. */
aatree_node_t *aatree_find_key(aatree_t *t, void *keyp,
                               aatree_condition_fun_t *cond);

/* Call f for each node inte the tree.
   If f returns false for any node, it will abort the iteration
   and aatree_each() returns false, otherwise true is returned. */
bool aatree_each(aatree_t *, bool (*f)(aatree_t *, aatree_node_t *));

/* Initialize an iterator for t.
   Returns false if tree is too deep, true otherwise. */
bool aatree_iter_init(aatree_t *t, aatree_iter_t *iter);
/* Get the next node from the iterator.
   Returns NULL when there is no more, or the tree is too deep. */
aatree_node_t *aatree_iter_next(aatree_iter_t *iter);

/* Initialize an iterator for matching keys in t.
   Returns false if not found, or the tree is too deep, true otherwise. */
bool aatree_iter_key_init(aatree_t *t, void *keyp, aatree_iter_t *iter);
/* Get the next matching node from the iterator.
   Returns NULL when there is no more, or the tree is too deep. */
aatree_node_t *aatree_iter_key_next(aatree_iter_t *iter);

/* Returns the height of the tree. */
uint64_t aatree_height(aatree_t *t);
