/* $Id: $
**
** Per-Erik Martin (pem@pem.nu) 2018-10-22
**
** Additional functions that implicitly use malloc/free for keys and nodes.
**
*/

#include <stdlib.h>
#include <string.h>

#include "aatree.h"
#include "aatree-internal.h"

aatree_t
aatreem_insert(aatree_t t, const char *key, void *value)
{
    aatree_t n = (aatree_t)malloc(sizeof(struct aatree_s));
    char *keycopy;

    if (n == NULL)
        return NULL;
    if ((keycopy = strdup(key)) == NULL)
    {
        free(n);
        return NULL;
    }
    aatree_init_node(n, keycopy, value);
    return aatree_insert_node(t, n);
}

aatree_t
aatreem_insert_unique(aatree_t t, const char *key, void *value, bool *uniquep)
{
    aatree_t n = (aatree_t)malloc(sizeof(struct aatree_s));
    char *keycopy;
    aatree_t newt;

    if (n == NULL)
        return NULL;
    if ((keycopy = strdup(key)) == NULL)
    {
        free(n);
        return NULL;
    }
    aatree_init_node(n, keycopy, value);
    newt = aatree_insert_unique_node(t, n, uniquep);
    if (! *uniquep)
    {
        free(keycopy);
        free(n);
    }
    return newt;
}

aatree_t
aatreem_replace(aatree_t t, const char *key, void *value,
                bool *replacedp, void **valuep)
{
    aatree_t n = (aatree_t)malloc(sizeof(struct aatree_s));
    char *keycopy;
    aatree_t newt;

    if (n == NULL)
        return NULL;
    if ((keycopy = strdup(key)) == NULL)
    {
        free(n);
        return NULL;
    }
    aatree_init_node(n, keycopy, value);
    newt = aatree_replace_node(t, n, replacedp, valuep);
    if (*replacedp)
    {
        free(keycopy);
        free(n);
    }
    return newt;
}

aatree_t
aatreem_delete(aatree_t t, const char *key,
               bool *deletedp, void **valuep)
{
    aatree_t node = NULL;

    t = aatree_remove_node(t, key, &node);
    if (deletedp != NULL)
        *deletedp = (node != NULL);
    if (node != NULL)
    {
        free(node->key);
        if (valuep != NULL)
            *valuep = node->value;
        free(node);
    }
    return t;
}

void
aatreem_destroy(aatree_t t, void (*freefun)(void *))
{
    while (t != NULL)
    {
        aatree_t left = t->left;
        aatree_t right = t->right;

        free(t->key);
        if (freefun != NULL)
            freefun(t->value);
        free(t);
        aatreem_destroy(left, freefun);
        t = right;
    }
}
