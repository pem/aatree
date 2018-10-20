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
aatree_delete_recursive(aatree_t t, int key,
                        aatree_t bottom, aatree_t *deletedp, bool *okp)
{
    aatree_t last;

    *okp = false;
    if (t != bottom)
    {
        last = t;
        if (key < t->key)
            t = aatree_delete_recursive(t->left, key,
                                        bottom, deletedp, okp);
        else
        {
            *deletedp = t;
            t = aatree_delete_recursive(t->right, key,
                                        bottom, deletedp, okp);
        }

        if (t == last && *deletedp != bottom && key == (*deletedp)->key)
        {
            (*deletedp)->key = t->key;
            *deletedp = bottom;
            t = t->right;
            free(last);
            *okp = true;
        }
        else if (t->left->level < t->level-1 ||
                 t->right->level < t->level-1)
        {
            t->level -= 1;
            if (t->right->level > t->level)
                t->right->level = t->level;
            t = aatree_skew(t);
            t->right = aatree_skew(t->right);
            t->right->right = aatree_skew(t->right->right);
            t = aatree_split(t);
            t->right = aatree_split(t->right);
        }
    }
    return t;
}

aatree_t
aatree_delete(aatree_t t, int key)
{
    bool ok;
    struct aatree_s bottom;
    aatree_t deleted = &bottom;

    bottom.left = bottom.right = &bottom;
    bottom.level = 0;

    return aatree_delete_recursive(t, key, &bottom, &deleted, &ok);
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
