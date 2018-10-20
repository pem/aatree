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
aatree_predecessor(aatree_t t)
{
    if (t == NULL || t->left == NULL)
        return NULL;
    for (t = t->left ; t->right != NULL ; t = t->right)
        ;
    return t;
}

static aatree_t
aatree_successor(aatree_t t)
{
    if (t == NULL || t->right == NULL)
        return NULL;
    for (t = t->right ; t->left != NULL ; t = t->left)
        ;
    return t;
}

aatree_t
aatree_delete(aatree_t t, int key)
{
    if (t == NULL)
        return NULL;
    if (key < t->key)
        t->left = aatree_delete(t->left, key);
    else if (key > t->key)
        t->right = aatree_delete(t->right, key);
    else
    {                           /* key == t->key */
        if (t->left == NULL && t->right == NULL)
            return NULL;        /* Leaf */
        if (t->left == NULL)
        {
            aatree_t tmp = aatree_successor(t);

            t->right = aatree_delete(t->right, tmp->key);
            t->key = tmp->key;
        }
        else
        {
            aatree_t tmp = aatree_predecessor(t);

            t->left = aatree_delete(t->left, tmp->key);
            t->key = tmp->key;
        }
    }

    if (t->left != NULL || t->right != NULL)
    {
        uint32_t should_be;

        if (t->left != NULL && t->right != NULL)
        {
            uint32_t lleft = t->left->level;
            uint32_t lright = t->right->level;
            should_be = (lleft < lright ? lleft : lright) + 1;
        }
        else if (t->left != NULL)
            should_be = t->left->level + 1;
        else
            should_be = t->right->level + 1;

        if (should_be < t->level)
        {
            t->level = should_be;
            if (should_be < t->right->level)
                t->right->level = should_be;
        }
    }

    t = aatree_skew(t);
    t->right = aatree_skew(t->right);
    if (t->right != NULL)
        t->right->right = aatree_skew(t->right->right);
    t = aatree_split(t);
    t->right = aatree_split(t->right);
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
        t = right;              /* Tail recursion */
    }
}
