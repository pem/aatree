/* $Id: $
**
** Per-Erik Martin (pem@foxt.com) 2018-10-22
**
** Additional functions that implicitly use malloc/free for keys and nodes.
**
*/

#include <stdlib.h>
#include <string.h>

#include "aatree.h"

aatree_t
aatree_insert(aatree_t t, const char *key, void *value)
{
    aatree_t n = (aatree_t)malloc(aatree_sizeof());
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
aatree_delete(aatree_t t, const char *key,
              bool *deletedp, void **valuep)
{
    aatree_t node = NULL;

    t = aatree_remove_node(t, key, &node);
    if (deletedp != NULL)
        *deletedp = (node != NULL);
    if (node != NULL)
    {
        free(aatree_key(node));
        if (valuep != NULL)
            *valuep = aatree_value(node);
        free(node);
    }
    return t;
}

void
aatree_destroy(aatree_t t)
{
    while (t != NULL)
    {
        aatree_t left = aatree_left(t);
        aatree_t right = aatree_right(t);

        free(aatree_key(t));
        free(t);
        aatree_destroy(left);   /* Deep recursion */
        t = right;
    }
}
