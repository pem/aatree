/*
** pem 2019-01-27
**
** Implicit malloc/free functions with a string key and void* value.
**
*/

#pragma once

#include "aatree.h"

/* Size is necessary in case we have expanded the struct; at
   least sizeof(aatree_t) will be allocated regardless of 'size'. */
aatree_t *aatreem_create(size_t);

char *aatree_key(aatree_node_t *t);

void *aatree_value(aatree_node_t *t);

aatree_node_t *aatree_left(aatree_node_t *t);

aatree_node_t*aatree_right(aatree_node_t *t);

aatree_level_t aatree_level(aatree_node_t *t);

/* Insert the key-value pair. A new node is allocated with malloc.
   Returns the new tree root. */
bool aatreem_insert(aatree_t *t, const char *key, void *value);

/* Insert the key-value pair. A new node is allocated with malloc.
   The key must be unique. If not, no insertion is done and *uniquep is
   set to false. *uniquep must be set to true before the call.
   Returns the new tree root. */
bool aatreem_insert_unique(aatree_t *t, const char *key, void *value,
                           void **xistsp);

/* Insert the key-value pair. If a node with the same key already
   exists, the value of that node is replaced and *valuep is set to
   the old value, otherwise it's inserted the normal way. *replacedp must
   be set to false before the call.
   Returns the new tree root. */
bool aatreem_replace(aatree_t *t, const char *key, void *value,
                     void **replacedp);

/* Delete the key-value node from the tree. *removedp is set to true
   if it was found (and removed), or false otherwise. *valuep is set
   to the value associated to the key. Both removep and valuep can
   be NULL. The removed node is freed.
   Returns the new tree root. */
bool aatreem_delete(aatree_t *t, const char *key, aatree_condition_fun_t *cond,
                    void **deletedp);

/* Destroy the tree by freeing all the nodes. If 'freefun' not NULL,
   it is called on each value pointer. */
void aatreem_destroy(aatree_t *t, void (*freefun)(void *));

/* Rename all occurences of 'oldkey' to 'newkey'. The tree is assumed to
   allow non-unique keys.
   QQQ Returns the new tree root. */
bool aatreem_rename(aatree_t *t, const char *oldkey, const char *newkey);
