/*
** Per-Erik Martin (pem@pem.nu) 2018-10-22
**
** Additional functions that implicitly use malloc/free for keys and nodes.
**
*/

#include <stdlib.h>
#include <string.h>

#include "aatreem.h"

#define UNUSED(x) ((void)(x))

typedef struct aatreem_node_s
{
    aatree_node_t n;
    char *key;
    void *value;
} aatreem_node_t;

char *
aatree_key(aatree_node_t *t)
{
    aatreem_node_t *n = (aatreem_node_t *)t;
    return n->key;
}

void *
aatree_value(aatree_node_t *t)
{
    aatreem_node_t *n = (aatreem_node_t *)t;
    return n->value;
}

aatree_node_t *
aatree_left(aatree_node_t *t)
{
    aatreem_node_t *n = (aatreem_node_t *)t;
    return n->n.left;
}

aatree_node_t *
aatree_right(aatree_node_t *t)
{
    aatreem_node_t *n = (aatreem_node_t *)t;
    return n->n.right;
}

aatree_level_t
aatree_level(aatree_node_t *t)
{
    aatreem_node_t *n = (aatreem_node_t *)t;
    return n->n.level;
}


bool
aatreem_insert(aatree_t *t, const char *key, void *value)
{
    aatreem_node_t *n = malloc(sizeof(aatreem_node_t));
    char *keycopy;

    if (n == NULL)
        return false;
    if ((keycopy = strdup(key)) == NULL)
    {
        free(n);
        return false;
    }
    aatree_init_node(&n->n);
    n->key = keycopy;
    n->value = value;
    aatree_insert_node(t, n->key, &n->n);
    return true;
}

bool
aatreem_insert_unique(aatree_t *t, const char *key, void *value,
                      void **xistsp)
{
    aatreem_node_t *n = malloc(sizeof(aatreem_node_t));
    char *keycopy;

    if (n == NULL)
        return false;
    if ((keycopy = strdup(key)) == NULL)
    {
        free(n);
        return false;
    }
    aatree_init_node(&n->n);
    n->key = keycopy;
    n->value = value;
    aatreem_node_t *xists =
        (aatreem_node_t *)aatree_insert_unique_node(t, n->key, &n->n);
    if (xistsp != NULL)
        *xistsp = (xists != NULL ? xists->value : NULL);
    if (xists != NULL)
    {
        free(keycopy);
        free(n);
        return false;
    }
    return true;
}

bool
aatreem_replace(aatree_t *t, const char *key, void *value,
                void **replacedp)
{
    aatreem_node_t *n = malloc(sizeof(aatreem_node_t));
    char *keycopy;

    if (n == NULL)
        return false;
    if ((keycopy = strdup(key)) == NULL)
    {
        free(n);
        return false;
    }
    aatree_init_node(&n->n);
    n->key = keycopy;
    n->value = value;
    aatreem_node_t *replaced =
        (aatreem_node_t *)aatree_replace_node(t, n->key, &n->n);
    if (replacedp != NULL)
        *replacedp = (replaced != NULL ? replaced->value : NULL);
    if (replaced != NULL)
    {
        free(replaced->key);
        free(n);
    }
    return true;
}

bool
aatreem_delete(aatree_t *t, const char *key, void **deletedp)
{
    aatreem_node_t *node = (aatreem_node_t *)aatree_remove_node(t, (void *)key);

    if (deletedp != NULL)
        *deletedp = (node != NULL ? node->value : NULL);
    if (node == NULL)
        return false;
    free(node->key);
    free(node);
    return true;
}

static int
aatreem_compare(aatree_t *t, void *keyp, aatree_node_t *b)
{
    UNUSED(t);
    char *key = keyp;
    aatreem_node_t *bm = (aatreem_node_t *)b;

    return strcmp(key, bm->key);
}

static void
aatreem_swap(aatree_t *t, aatree_node_t *a, aatree_node_t *b)
{
    UNUSED(t);
    aatreem_node_t *am = (aatreem_node_t *)a;
    aatreem_node_t *bm = (aatreem_node_t *)b;
    char *tmpkey;
    void *tmpval;

    tmpkey = am->key;
    am->key = bm->key;
    bm->key = tmpkey;
    tmpval = am->value;
    am->value = bm->value;
    bm->value = tmpval;
}

aatree_t *
aatreem_create(void)
{
    aatree_t *t = malloc(sizeof(aatree_t));

    t->root = NULL;
    t->compare = aatreem_compare;
    t->swap = aatreem_swap;
    return t;
}

static void
aatreem_destroy_rec(aatree_node_t *t, void (*freefun)(void *))
{
    while (t != NULL)
    {
        aatree_node_t *left = t->left;
        aatree_node_t *right = t->right;
        aatreem_node_t *n = (aatreem_node_t *)t;

        free(n->key);
        if (freefun != NULL)
            freefun(n->value);
        free(n);
        aatreem_destroy_rec(left, freefun);
        t = right;
    }
}

void
aatreem_destroy(aatree_t *t, void (*freefun)(void *))
{
    aatreem_destroy_rec(t->root, freefun);
    free(t);
}

bool
aatreem_rename(aatree_t *t, const char *oldkey, const char *newkey)
{
    aatreem_node_t *n = (aatreem_node_t *)t->root;

    while (n != NULL)
    {
        char *keycopy;
        aatreem_node_t *deleted =
            (aatreem_node_t *)aatree_remove_node(t, (void *)oldkey);

        if (deleted == NULL)
            break;              /* Done */
        deleted->n.left = deleted->n.right = NULL;
        keycopy = strdup(newkey);
        if (keycopy == NULL)
            return false;
        free(deleted->key);
        deleted->key = keycopy;
        aatree_insert_node(t, keycopy, (aatree_node_t *)deleted);
    }
    return true;
}
