/*
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
*/

#ifndef _aatree_h_
#define _aatree_h_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct aatree_s *aatree_t;

/* More than we need really, 16 bits should be enough, maybe even 8.
   In theory, the number of bits needed is log2(h) where h is the
   height of the tree. */
typedef uint32_t aatree_level_t;
#define AATREE_MAX_DEPTH 32
typedef struct aatree_iter_s
{
    const char *key;
    uint32_t i;
    aatree_t node[AATREE_MAX_DEPTH];
} aatree_iter_t;

/* Returns the size of a node, used for allocating space for a new node. */
size_t aatree_sizeof(void);

/* Initializes a new node. */
void aatree_init_node(aatree_t n, char *key, void *value);

char *aatree_key(aatree_t t);

void *aatree_value(aatree_t t);

aatree_t aatree_left(aatree_t t);

aatree_t aatree_right(aatree_t t);

aatree_level_t aatree_level(aatree_t t);

/* Insert the node into the tree.
   Returns the new tree root. */
aatree_t aatree_insert_node(aatree_t t, aatree_t n);

/* Insert the node into the tree. The key must be unique. If not,
   no insertion is done and *uniquep is set to false. *uniquep must be
   set to true before the call.
   Returns the new tree root. */
aatree_t aatree_insert_unique_node(aatree_t t, aatree_t n, bool *uniquep);

/* Insert the node into the tree. If a node with the same key already
   exists, the value of that node is replaced and *valuep is set to
   the old value and *replacedp to true, otherwise it's inserted the normal way.
   Note that when replaced, the node 'n' will is unused, only its value
   is copied. *replacedp must be set to false before the call.
   Returns the new tree root. */
aatree_t aatree_replace_node(aatree_t t, aatree_t n,
                             bool *replacedp, void **valuep);

/* Remove a node with matching key. *nodep is set to the removed node
   if it was found, or NULL otherwise. It will remove the first matching
   node it encounters in the tree.
   Returns the new tree root. */
aatree_t aatree_remove_node(aatree_t t, const char *key, aatree_t *nodep);

/* Find a node matching 'key'. The comparison is done with strcmp().
   It will return the first matching node it encounters in the tree.
   Returns the found node, or NULL. */
aatree_t aatree_find_key(aatree_t t, const char *key);

/* Call f for each node inte the tree.
   If f returns false for any node, it will abort the iteration
   and aatree_each() returns false, otherwise true is returned. */
bool aatree_each(aatree_t, bool (*f)(aatree_t));

/* Initialize an iterator for t.
   Returns false if tree is too deep, true otherwise. */
bool aatree_iter_init(aatree_t t, aatree_iter_t *iter);
/* Get the next node from the iterator.
   Returns NULL when there is no more, or the tree is too deep. */
aatree_t aatree_iter_next(aatree_iter_t *iter);

/* Initialize an iterator for matching keys in t.
   Returns false if not found, or the tree is too deep, true otherwise. */
bool aatree_iter_key_init(aatree_t t, const char *key, aatree_iter_t *iter);
/* Get the next matching node from the iterator.
   Returns NULL when there is no more, or the tree is too deep. */
aatree_t aatree_iter_key_next(aatree_iter_t *iter);


/*
** Implicit malloc/free functions
*/

/* Insert the key-value pair. A new node is allocated with malloc.
   Returns the new tree root. */
aatree_t aatreem_insert(aatree_t t, const char *key, void *value);

/* Insert the key-value pair. A new node is allocated with malloc.
   The key must be unique. If not, no insertion is done and *uniquep is
   set to false. *uniquep must be set to true before the call.
   Returns the new tree root. */
aatree_t aatreem_insert_unique(aatree_t t, const char *key, void *value,
                               bool *uniquep);

/* Insert the key-value pair. If a node with the same key already
   exists, the value of that node is replaced and *valuep is set to
   the old value, otherwise it's inserted the normal way. *replacedp must
   be set to false before the call.
   Returns the new tree root. */
aatree_t aatreem_replace(aatree_t t, const char *key, void *value,
                         bool *replacedp, void **valuep);

/* Delete the key-value node from the tree. *removedp is set to true
   if it was found (and removed), or false otherwise. *valuep is set
   to the value associated to the key. Both removep and valuep can
   be NULL. The removed node is freed.
   Returns the new tree root. */
aatree_t aatreem_delete(aatree_t t, const char *key,
                        bool *removedp, void **valuep);

/* Destroy the tree by freeing all the nodes. If 'freefun' not NULL,
   it is called on each value pointer. */
void aatreem_destroy(aatree_t t, void (*freefun)(void *));

#endif /* _aatree_h_ */
