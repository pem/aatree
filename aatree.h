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

int aatree_key(aatree_t t);

aatree_t aatree_left(aatree_t t);

aatree_t aatree_right(aatree_t t);

uint32_t aatree_level(aatree_t t);

aatree_t aatree_insert(aatree_t t, int key);

aatree_t aatree_delete(aatree_t t, int key);

bool aatree_search(aatree_t t, int key);

void aatree_each(aatree_t, void (*f)(aatree_t));

void aatree_destroy(aatree_t t);

#endif /* _aatree_h_ */
