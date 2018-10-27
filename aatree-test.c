/* $Id: $
**
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "aatree.h"

static void
ptree(aatree_t n, int indent)
{
    if (n != NULL)
    {
        char *val = aatree_value(n);

        ptree(aatree_right(n), indent+1);
        for (int i = indent ; i > 0 ; i--)
            printf("  ");
        if (val == NULL)
            printf("(%u)%s\n", (unsigned)aatree_level(n), aatree_key(n));
        else
            printf("(%u)%s:%s\n",
                   (unsigned)aatree_level(n), aatree_key(n), val);
        ptree(aatree_left(n), indent+1);
    }
}
    
static bool
pnode(aatree_t n)
{
    char *val = aatree_value(n);

    if (val == NULL)
        printf(" %s", aatree_key(n));
    else
        printf(" %s:%s", aatree_key(n), val);
    return true;
}

/* Check invariants for AA Trees */
static bool
cnode(aatree_t n)
{
    char *key = aatree_key(n);
    uint32_t level = aatree_level(n);
    aatree_t left = aatree_left(n);
    aatree_t right = aatree_right(n);

    /* 1. The level of every leaf node is one */
    if (left == NULL && right == NULL && level != 1)
        printf("1: Leaf node %s has level %u\n", key, (unsigned)level);
    /* 2. The level of every left child is exactly one less than that of
       its parent */
    if (left != NULL)
    {
        uint32_t llevel = aatree_level(left);

        if (llevel != level-1)
            printf("2: Left child %s of %s has level %u, not %u\n",
                   aatree_key(left), key, (unsigned)llevel,
                   (unsigned)(level-1));
    }
    /* 3. The level of very right child is equal to, or one less than, that
       of its parent */
    if (right != NULL)
    {
        uint32_t rlevel = aatree_level(right);

        if (rlevel != level && rlevel != level-1)
            printf("3: Right child %s of %s has level %u, not %u or %u\n",
                   aatree_key(right), key, (unsigned)rlevel,
                   (unsigned)level, (unsigned)(level-1));
        /* 4. The level of every right grandchild is strictly less than that
           of its grandparent */
        aatree_t gright = aatree_right(right);

        if (gright != NULL)
        {
            uint32_t glevel = aatree_level(gright);

            if (glevel >= level)
                printf("4: Right grandchild %s of %s has level %u, not < %u\n",
                       aatree_key(gright), key, (unsigned)glevel,
                       (unsigned)level);
        }
    }
    /* 5. Every node of level greater than one has two children */
    if (level > 1 && (left == NULL || right == NULL))
        printf("5: Node %s with level %u has one or none children\n",
               key, (unsigned)level);
    return true;
}

int
main(int argc, char **argv)
{
    int c;
    char *delkey, *findkey;
    bool verbose = false, delete = false, find = false;
    aatree_t root = NULL;

    opterr = 0;
    while ((c = getopt(argc, argv, "d:f:v")) != EOF)
        switch (c)
        {
        case 'd':
            delete = true;
            delkey = optarg;
            break;
        case 'f':
            find = true;
            findkey = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        default:
            fprintf(stderr, "aatree-test [-d <key>] [-f <key] keys...\n");
            exit(1);
        }
    for (int i = optind ; i < argc ; i++)
    {
        char *key = strdup(argv[i]);
        char *val = strchr(key, ':');

        if (val != NULL)
            *val++ = '\0';
        root = aatree_insert(root, key, val);
        if (verbose)
        {
            ptree(root, 0);
            printf("--------------------\n");
        }
        (void)aatree_each(root, cnode);
    }
    if (! verbose)
    {
        ptree(root, 0);
        printf("--------------------\n");
    }
    printf("Each:");
    (void)aatree_each(root, pnode);
    printf("\n--------------------\n");
    printf("Iter:");
    {
        aatree_iter_t iter;
        aatree_t n;
        if (! aatree_iter_init(root, &iter))
            fprintf(stderr, "Tree is too deep for iterator\n");
        else
            while ((n = aatree_iter_next(&iter)) != NULL)
                (void)pnode(n);
    }
    printf("\n--------------------\n");

    if (find)
    {
        aatree_t n;
        aatree_iter_t iter;

        printf("Find: %s\n", findkey);
        if ((n = aatree_find_key(root, findkey)) == NULL)
            printf("  Not found\n");
        else
        {
            char *val = aatree_value(n);

            printf("  Found %s\n", (val == NULL ? "(null)" : val));
        }
        printf("--------------------\n");
        printf("Iter find: %s\n", findkey);
        if (! aatree_iter_key_init(root, findkey, &iter))
            printf("  Iter not found\n");
        else
        {
            while ((n = aatree_iter_key_next(&iter)) != NULL)
            {
                char *val = aatree_value(n);

                printf("  Found %s\n", (val == NULL ? "(null)" : val));
            }
        }
        printf("--------------------\n");
    }
    if (delete)
    {
        bool deleted = false;

        printf("Deleting: %s\n", delkey);
        printf("--------------------\n");
        root = aatree_delete(root, delkey, &deleted, NULL);
        if (! deleted)
        {
            printf("Not deleted\n");
            printf("--------------------\n");
        }
        else
        {
            (void)aatree_each(root, cnode);
            ptree(root, 0);
            printf("--------------------\n");
            printf("Order:");
            (void)aatree_each(root, pnode);
            printf("\n--------------------\n");
        }
    }

    aatree_destroy(root);

    exit(0);
}
