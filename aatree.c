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

void
aatree_init_node(aatree_node_t *n)
{
    memset(n, 0, sizeof(struct aatree_s));
    n->level = 1;
}

static aatree_node_t *
aatree_skew(aatree_node_t *t)
{
    if (t == NULL)
        return NULL;
    if (t->left == NULL || t->level != t->left->level)
        return t;
    aatree_node_t *tmp = t;
    t = t->left;
    tmp->left = t->right;
    t->right = tmp;
    return t;
}

static aatree_node_t *
aatree_split(aatree_node_t *t)
{
    if (t == NULL)
        return NULL;
    if (t->right == NULL || t->right->right == NULL ||
        t->level != t->right->right->level)
        return t;
    aatree_node_t *tmp = t;
    t = t->right;
    tmp->right = t->left;
    t->left = tmp;
    t->level += 1;
    return t;
}

static aatree_node_t *
insert_node(aatree_t *b, aatree_node_t *t, void *keyp, aatree_node_t *n)
{
    if (t == NULL)
        return n;
    if (b->compare(b, keyp, t) < 0)
        t->left = insert_node(b, t->left, keyp, n);
    else
        t->right = insert_node(b, t->right, keyp, n);
    t = aatree_skew(t);
    t = aatree_split(t);
    return t;
}

void
aatree_insert_node(aatree_t *t, void *keyp, aatree_node_t *n)
{
    t->root = insert_node(t, t->root, keyp, n);
}

static aatree_node_t *
insert_unique_node(aatree_t *b, aatree_node_t *t,
                   void *keyp, aatree_node_t *n,  aatree_node_t **xistsp)
{
    if (t == NULL)
        return n;
    int cmp = b->compare(b, keyp, t);
    if (cmp == 0)
    {
        *xistsp = t;
        return t;
    }
    if (cmp < 0)
        t->left = insert_unique_node(b, t->left, keyp, n, xistsp);
    else
        t->right = insert_unique_node(b, t->right, keyp, n, xistsp);
    t = aatree_skew(t);
    t = aatree_split(t);
    return t;
}

aatree_node_t *
aatree_insert_unique_node(aatree_t *t, void *keyp, aatree_node_t *n)
{
    aatree_node_t *xists = NULL;

    t->root = insert_unique_node(t, t->root, keyp, n, &xists);
    return xists;
}

static aatree_node_t *
replace_node(aatree_t *b, aatree_node_t *t,
             void *keyp, aatree_node_t *n, aatree_node_t **replp)
{
    if (t == NULL)
        return n;
    int cmp = b->compare(b, keyp, t);
    if (cmp == 0)
    {
        b->swap(b, n, t);
        *replp = n;
        return t;
    }
    if (cmp < 0)
        t->left = replace_node(b, t->left, keyp, n, replp);
    else
        t->right = replace_node(b, t->right, keyp, n, replp);
    t = aatree_skew(t);
    t = aatree_split(t);
    return t;
}

aatree_node_t *
aatree_replace_node(aatree_t *t, void *keyp, aatree_node_t *n)
{
    aatree_node_t *repl = NULL;

    t->root = replace_node(t, t->root, keyp, n, &repl);
    return repl;
}

/* Correct the levels and re-balance; refer to the original article or other
   texts about AA Trees for details. */
static aatree_node_t *
aatree_post_remove_fix(aatree_node_t *t)
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

static aatree_node_t *
remove_find_successor(aatree_t *b,
                      aatree_node_t *t, aatree_node_t *found,
                      aatree_node_t **removedp)
{
    if (t->left == NULL)
    {                           /* Found successor */
        b->swap(b, found, t);
        *removedp = t;
        return t->right;
    }
    t->left = remove_find_successor(b, t->left, found, removedp);
    return aatree_post_remove_fix(t);
}

/* Will an AA Tree ever have such a shape that we have to find the
   predecessor? Don't know, so we'll keep this just in case... */
static aatree_node_t *
remove_find_predecessor(aatree_t *b,
                        aatree_node_t *t, aatree_node_t *found,
                        aatree_node_t **removedp)
{
    if (t->right == NULL)
    {                           /* Found predecessor */
        b->swap(b, found, t);
        *removedp = t;
        return t->left;
    }
    t->right = remove_find_predecessor(b, t->right, found, removedp);
    return aatree_post_remove_fix(t);
}

static aatree_node_t *
remove_recursive(aatree_t *b,
                 aatree_node_t *t, void *keyp, aatree_condition_fun_t *cond,
                 aatree_node_t **removedp)
{
    if (t == NULL)
        return NULL;            /* Not found */
    int cmp = b->compare(b, keyp, t);
    if (cmp == 0 && (cond == NULL || cond(b, t)))
    {                           /* Found it */
        if (t->right != NULL)   /* Pick right branch, if any */
            t->right = remove_find_successor(b, t->right, t, removedp);
        else if (t->left != NULL) /* Will this ever happen? */
            t->left = remove_find_predecessor(b, t->left, t, removedp);
        else
        {                       /* Found a leaf */
            *removedp = t;
            return NULL;
        }
    }
    else
    {                           /* Keep looking */
        if (t->left != NULL && cmp < 0)
            t->left = remove_recursive(b,t->left, keyp, cond, removedp);
        else if (t->right != NULL)
            t->right = remove_recursive(b, t->right, keyp, cond, removedp);
        else
            return t;           /* A leaf */
    }
    return aatree_post_remove_fix(t);
}

aatree_node_t *
aatree_remove_node(aatree_t *t, void *keyp, aatree_condition_fun_t *cond)
{
    aatree_node_t *node = NULL;

    t->root = remove_recursive(t, t->root, keyp, cond, &node);
    return node;
}

aatree_node_t *
aatree_find_key(aatree_t *t, void *key)
{
    aatree_node_t *n = t->root;

    while (n != NULL)
    {
        int cmp = t->compare(t, key, n);

        if (cmp == 0)
            break;
        if (cmp < 0)
            n = n->left;
        else
            n = n->right;
    }
    return n;
}

static bool
each(aatree_t *b, aatree_node_t *t, bool (*f)(aatree_t *, aatree_node_t *))
{
    while (t != NULL)
    {
        if (! each(b, t->left, f))
            return false;
        if (! f(b, t))
            return false;
        t = t->right;
    }
    return true;
}

bool
aatree_each(aatree_t *t, bool (*f)(aatree_t *, aatree_node_t *))
{
    return each(t, t->root, f);
}

bool
aatree_iter_init(aatree_t *t, aatree_iter_t *iter)
{
    aatree_node_t *n = t->root;

    memset(iter, 0, sizeof(aatree_iter_t));
    while (n != NULL)
    {
        if (iter->i >= AATREE_MAX_DEPTH)
            return false;
        iter->node[iter->i++] = n;
        n = n->left;
    }
    return true;
}

aatree_node_t *
aatree_iter_next(aatree_iter_t *iter)
{
    if (iter->i == 0)
        return NULL;
    aatree_node_t *t = iter->node[--iter->i];
    for (aatree_node_t *tr = t->right ; tr != NULL ; tr = tr->left)
    {
        if (iter->i >= AATREE_MAX_DEPTH)
            return NULL;
        iter->node[iter->i++] = tr;
    }
    return t;
}

bool
aatree_iter_key_init(aatree_t *t, void *key, aatree_iter_t *iter)
{
    memset(iter, 0, sizeof(aatree_iter_t));
    iter->keyp = key;
    iter->base = t;
    if (t->root != NULL)
        iter->node[iter->i++] = t->root;
    return true;
}

aatree_node_t *
aatree_iter_key_next(aatree_iter_t *iter)
{
    aatree_node_t *t = NULL;

    while (iter->i > 0)
    {
        t = iter->node[--iter->i];
        while (t != NULL)
        {
            int cmp = iter->base->compare(iter->base, iter->keyp, t);

            if (cmp == 0)
                break;
            if (cmp < 0)
                t = t->left;
            else
                t = t->right;
        }
        if (t != NULL)
            break;
    }
    if (t == NULL)
        return NULL;
    if (t->right != NULL)
    {
        if (iter->i >= AATREE_MAX_DEPTH)
            return NULL;
        iter->node[iter->i++] = t->right;
    }
    if (t->left != NULL)
    {
        if (iter->i >= AATREE_MAX_DEPTH)
            return NULL;
        iter->node[iter->i++] = t->left;
    }
    return t;
}
