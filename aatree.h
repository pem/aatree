/* $Id: $
**
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
*/

#ifndef _aatree_h_
#define _aatree_h_

#include <stdint.h>
#include <stdbool.h>

typedef struct aatree_s *aatree_t;

/* More than we need really, 16 bits should be enough, maybe even 8.
   In theory, the number of bits needed is log2(h) where h is the
   height of the tree. */
typedef uint32_t aatree_level_t;

char *aatree_key(aatree_t t);

void *aatree_value(aatree_t t);

aatree_t aatree_left(aatree_t t);

aatree_t aatree_right(aatree_t t);

aatree_level_t aatree_level(aatree_t t);

aatree_t aatree_insert(aatree_t t, const char *key, void *value);

aatree_t aatree_delete(aatree_t t, const char *key,
                       bool *deletedp, void **valuep);

bool aatree_search(aatree_t t, const char *key, void **valuep);

void aatree_each(aatree_t, void (*f)(aatree_t));

void aatree_destroy(aatree_t t);

#endif /* _aatree_h_ */
