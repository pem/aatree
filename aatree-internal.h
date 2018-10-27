/*
** Per-Erik Martin (pem@pem.nu) 2018-10-27
**
*/

#ifndef _aatree_internal_h_
#define _aatree_internal_h_

struct aatree_s
{
    struct aatree_s *left, *right;
    aatree_level_t level;
    char *key;
    void *value;
};

#endif /* _aatree_internal_h_ */
