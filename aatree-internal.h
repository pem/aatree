/*
** Per-Erik Martin (pem@pem.nu) 2018-10-27
**
*/

#pragma once

struct aatree_s
{
    struct aatree_s *left, *right;
    aatree_level_t level;
    char *key;
    void *value;
};
