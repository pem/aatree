/*
**
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
** See "Balanced Search Trees Made Simple" by Arne Andersson,
** http://user.it.uu.se/~arnea/ps/simp.pdf .
** http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_andersson.aspx
*/

#include <stdlib.h>
#include <string.h>

#include "aatree.h"

struct aatree_s
{
    struct aatree_s *left, *right;
    uint32_t level;
    int key;
};

int
aatree_key(aatree_t t)
{
    return t->key;
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

uint32_t
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

static aatree_t
aatree_insert_node(aatree_t t, aatree_t n)
{
    if (t == NULL)
        return n;
    if (n->key < t->key)
        t->left = aatree_insert_node(t->left, n); /* Deep recursion */
    else
        t->right = aatree_insert_node(t->right, n); /* Deep recursion */
    t = aatree_skew(t);
    t = aatree_split(t);
    return t;
}

aatree_t
aatree_insert(aatree_t t, int key)
{
    aatree_t n = (aatree_t)malloc(sizeof(struct aatree_s));

    if (n == NULL)
        return NULL;
    memset(n, 0, sizeof(struct aatree_s));
    n->key = key;
    n->level = 1;
    return aatree_insert_node(t, n);
}

static aatree_t
aatree_post_delete_fix(aatree_t t)
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

static void
aatree_swap(aatree_t found, aatree_t leaf)
{
    int key = found->key;

    found->key = leaf->key;
    leaf->key = key;
}

static aatree_t
aatree_delete_find_successor(aatree_t t, aatree_t found, aatree_t *deletedp)
{
    if (t->left == NULL)
    {                           /* Found successor */
        aatree_swap(found, t);
        *deletedp = t;
        return t->right;
    }
    t->left = aatree_delete_find_successor(t->left, found, deletedp);
    return aatree_post_delete_fix(t);
}

static aatree_t
aatree_delete_find_predecessor(aatree_t t, aatree_t found, aatree_t *deletedp)
{
    if (t->right == NULL)
    {                           /* Found successor */
        aatree_swap(found, t);
        *deletedp = t;
        return t->left;
    }
    t->right = aatree_delete_find_predecessor(t->right, found, deletedp);
    return aatree_post_delete_fix(t);
}

static aatree_t
aatree_delete_recursive(aatree_t t, int key, aatree_t *deletedp)
{
    if (t == NULL)
        return NULL;            /* Not found */
    if (key == t->key)
    {                           /* Found it */
        if (t->right != NULL)
            t->right = aatree_delete_find_successor(t->right, t, deletedp);
        else if (t->left != NULL)
            t->left = aatree_delete_find_predecessor(t->left, t, deletedp);
        else
        {                       /* Found a leaf */
            *deletedp = t;
            return NULL;
        }
    }
    else
    {                           /* Keep looking */
        if (t->left != NULL && key < t->key)
            t->left = aatree_delete_recursive(t->left, key, deletedp);
        else if (t->right != NULL)
            t->right = aatree_delete_recursive(t->right, key, deletedp);
        else
            return t;           /* A leaf */
    }
    return aatree_post_delete_fix(t);
}

aatree_t
aatree_delete(aatree_t t, int key)
{
    aatree_t deleted = NULL;

    t = aatree_delete_recursive(t, key, &deleted);
    free(deleted);
    return t;
}

bool
aatree_search(aatree_t t, int key)
{
    while (t != NULL)
    {
        if (key == t->key)
            return true;
        if (key < t->key)
            t = t->left;
        else
            t = t->right;
    }
    return false;
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

void
aatree_destroy(aatree_t t)
{
    while (t != NULL)
    {
        aatree_t left = t->left;
        aatree_t right = t->right;

        free(t);
        aatree_destroy(left);   /* Deep recursion */
        t = right;
    }
}
