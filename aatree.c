/*
**
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
** See "Balanced Search Trees Made Simple" by Arne Andersson,
** http://user.it.uu.se/~arnea/ps/simp.pdf .
**
*/

#include <stdlib.h>
#include <string.h>

#include "aatree.h"

struct aatree_s
{
    struct aatree_s *left, *right;
    aatree_level_t level;
    char *key;
    void *value;
};

size_t
aatree_sizeof(void)
{
    return sizeof(struct aatree_s);
}

void
aatree_init_node(aatree_t n, char *key, void *value)
{
    memset(n, 0, sizeof(struct aatree_s));
    n->key = key;
    n->value = value;
    n->level = 1;
}

char *
aatree_key(aatree_t t)
{
    return t->key;
}

void *
aatree_value(aatree_t t)
{
    return t->value;
}

aatree_t
aatree_left(aatree_t t)
{
    return t->left;
}

aatree_t
aatree_right(aatree_t t)
{
    return t->right;
}

aatree_level_t
aatree_level(aatree_t t)
{
    return t->level;
}

static aatree_t
aatree_skew(aatree_t t)
{
    if (t == NULL)
        return NULL;
    if (t->left == NULL || t->level != t->left->level)
        return t;
    aatree_t tmp = t;
    t = t->left;
    tmp->left = t->right;
    t->right = tmp;
    return t;
}

static aatree_t
aatree_split(aatree_t t)
{
    if (t == NULL)
        return NULL;
    if (t->right == NULL || t->right->right == NULL ||
        t->level != t->right->right->level)
        return t;
    aatree_t tmp = t;
    t = t->right;
    tmp->right = t->left;
    t->left = tmp;
    t->level += 1;
    return t;
}

aatree_t
aatree_insert_node(aatree_t t, aatree_t n)
{
    if (t == NULL)
        return n;
    if (strcmp(n->key, t->key) < 0)
        t->left = aatree_insert_node(t->left, n); /* Deep recursion */
    else
        t->right = aatree_insert_node(t->right, n); /* Deep recursion */
    t = aatree_skew(t);
    t = aatree_split(t);
    return t;
}

/* Correct the levels and re-balance; refer to the original article or other
   texts about AA Trees for details. */
static aatree_t
aatree_post_remove_fix(aatree_t t)
{
    if (t != NULL)
    {
        if (t->left == NULL || t->right == NULL)
            t->level = 1;
        if (t->left != NULL && t->level > t->left->level+1)
            t->level -= 1;
        if (t->right != NULL)
        {
            if (t->level > t->right->level+1)
                t->level -= 1;
            if (t->level < t->right->level)
                t->right->level = t->level;
        }
        t = aatree_skew(t);
        if (t->right != NULL)
        {
            t->right = aatree_skew(t->right);
            if (t->right != NULL)
                t->right->right = aatree_skew(t->right->right);
        }
        t = aatree_split(t);
        if (t->right != NULL)
            t->right = aatree_split(t->right);
    }
    return t;
}

/* Technically it's only necessary to update the found node, but we swap
   the values so we can return the removed key and value to the caller. */
static void
aatree_swap(aatree_t found, aatree_t leaf)
{
    char *key = found->key;
    void *value = found->value;

    found->key = leaf->key;
    found->value = leaf->value;
    leaf->key = key;
    leaf->value = value;
}

static aatree_t
aatree_remove_find_successor(aatree_t t, aatree_t found, aatree_t *removedp)
{
    if (t->left == NULL)
    {                           /* Found successor */
        aatree_swap(found, t);
        *removedp = t;
        return t->right;
    }
    t->left = aatree_remove_find_successor(t->left, found, removedp);
    return aatree_post_remove_fix(t);
}

/* Will an AA Tree ever have such a shape that we have to find the
   predecessor? Don't know, so we'll keep this just in case... */
static aatree_t
aatree_remove_find_predecessor(aatree_t t, aatree_t found, aatree_t *removedp)
{
    if (t->right == NULL)
    {                           /* Found predecessor */
        aatree_swap(found, t);
        *removedp = t;
        return t->left;
    }
    t->right = aatree_remove_find_predecessor(t->right, found, removedp);
    return aatree_post_remove_fix(t);
}

static aatree_t
aatree_remove_recursive(aatree_t t, const char *key, aatree_t *removedp)
{

    if (t == NULL)
        return NULL;            /* Not found */
    int cmp = strcmp(key, t->key);
    if (cmp == 0)
    {                           /* Found it */
        if (t->right != NULL)
            t->right = aatree_remove_find_successor(t->right, t, removedp);
        else if (t->left != NULL)
            t->left = aatree_remove_find_predecessor(t->left, t, removedp);
        else
        {                       /* Found a leaf */
            *removedp = t;
            return NULL;
        }
    }
    else
    {                           /* Keep looking */
        if (t->left != NULL && cmp < 0)
            t->left = aatree_remove_recursive(t->left, key, removedp);
        else if (t->right != NULL)
            t->right = aatree_remove_recursive(t->right, key, removedp);
        else
            return t;           /* A leaf */
    }
    return aatree_post_remove_fix(t);
}

aatree_t
aatree_remove_node(aatree_t t, const char *key, aatree_t *nodep)
{
    aatree_t node = NULL;

    t = aatree_remove_recursive(t, key, &node);
    if (nodep != NULL)
        *nodep = node;
    return t;
}

aatree_t
aatree_find_key(aatree_t t, const char *key)
{
    while (t != NULL)
    {
        int cmp = strcmp(key, t->key);

        if (cmp == 0)
            break;
        if (cmp < 0)
            t = t->left;
        else
            t = t->right;
    }
    return t;
}

void
aatree_each(aatree_t t, void (*f)(aatree_t))
{
    while (t != NULL)
    {
        aatree_each(t->left, f); /* Deep recursion */
        f(t);
        t = t->right;
    }
}
