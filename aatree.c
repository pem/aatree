/*
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
** See "Balanced Search Trees Made Simple" by Arne Andersson,
** http://user.it.uu.se/~arnea/ps/simp.pdf .
**
*/

#include <stdlib.h>
#include <string.h>

#include "aatree.h"
#include "aatree-internal.h"

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
        t->left = aatree_insert_node(t->left, n);
    else
        t->right = aatree_insert_node(t->right, n);
    t = aatree_skew(t);
    t = aatree_split(t);
    return t;
}

aatree_t
aatree_insert_unique_node(aatree_t t, aatree_t n,  aatree_t *xistsp)
{
    if (t == NULL)
        return n;
    int cmp = strcmp(n->key, t->key);
    if (cmp == 0)
    {
        *xistsp = t;
        return t;
    }
    if (cmp < 0)
        t->left = aatree_insert_unique_node(t->left, n, xistsp);
    else
        t->right = aatree_insert_unique_node(t->right, n, xistsp);
    t = aatree_skew(t);
    t = aatree_split(t);
    return t;
}

aatree_t
aatree_replace_node(aatree_t t, aatree_t n, bool *replacedp, void **valuep)
{
    if (t == NULL)
        return n;
    int cmp = strcmp(n->key, t->key);
    if (cmp == 0)
    {
        *replacedp = true;
        if (valuep)
            *valuep = t->value;
        t->value = n->value;
        return t;
    }
    if (cmp < 0)
        t->left = aatree_replace_node(t->left, n, replacedp, valuep);
    else
        t->right = aatree_replace_node(t->right, n, replacedp, valuep);
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
        if (t->right != NULL)   /* Pick right branch, if any */
            t->right = aatree_remove_find_successor(t->right, t, removedp);
        else if (t->left != NULL) /* Will this ever happen? */
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

bool
aatree_each(aatree_t t, bool (*f)(aatree_t))
{
    while (t != NULL)
    {
        if (! aatree_each(t->left, f))
            return false;
        if (! f(t))
            return false;
        t = t->right;
    }
    return true;
}

bool
aatree_iter_init(aatree_t t, aatree_iter_t *iter)
{
    memset(iter, 0, sizeof(aatree_iter_t));
    while (t != NULL)
    {
        if (iter->i >= AATREE_MAX_DEPTH)
            return false;
        iter->node[iter->i++] = t;
        t = t->left;
    }
    return true;
}

aatree_t
aatree_iter_next(aatree_iter_t *iter)
{
    if (iter->i == 0)
        return NULL;
    aatree_t t = iter->node[--iter->i];
    for (aatree_t tr = t->right ; tr != NULL ; tr = tr->left)
    {
        if (iter->i >= AATREE_MAX_DEPTH)
            return NULL;
        iter->node[iter->i++] = tr;
    }
    return t;
}

bool
aatree_iter_key_init(aatree_t t, const char *key, aatree_iter_t *iter)
{
    memset(iter, 0, sizeof(aatree_iter_t));
    iter->key = key;
    t = aatree_find_key(t, key);
    if (t == NULL)
        return false;
    while (t != NULL)
    {
        if (strcmp(key, t->key) != 0)
            break;
        if (iter->i >= AATREE_MAX_DEPTH)
            return false;
        iter->node[iter->i++] = t;
        t = t->left;
    }
    return true;
}

aatree_t
aatree_iter_key_next(aatree_iter_t *iter)
{
    if (iter->i == 0)
        return NULL;
    aatree_t t = iter->node[--iter->i];
    for (aatree_t tr = t->right ; tr != NULL ; tr = tr->left)
    {
        if (strcmp(iter->key, tr->key) != 0)
            break;
        if (iter->i >= AATREE_MAX_DEPTH)
            return NULL;
        iter->node[iter->i++] = tr;
    }
    return t;
}
