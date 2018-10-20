/* $Id: $
**
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "aatree.h"

static void
ptree(aatree_t n, int indent)
{
    if (n != NULL)
    {
        ptree(aatree_right(n), indent+1);
        for (int i = indent ; i > 0 ; i--)
            printf("  ");
        printf("%u:%d\n", (unsigned)aatree_level(n), aatree_key(n));
        ptree(aatree_left(n), indent+1);
    }
}
    
static void
pnode(aatree_t n)
{
    printf(" %d", aatree_key(n));
}

/* Check invariants for AA Trees */
static void
cnode(aatree_t n)
{
    int key = aatree_key(n);
    uint32_t level = aatree_level(n);
    aatree_t left = aatree_left(n);
    aatree_t right = aatree_right(n);

    /* 1. The level of every leaf node is one */
    if (left == NULL && right == NULL && level != 1)
        printf("1: Leaf node %d has level %u\n", key, (unsigned)level);
    /* 2. The level of every left child is exactly one less than that of
       its parent */
    if (left != NULL)
    {
        uint32_t llevel = aatree_level(left);

        if (llevel != level-1)
            printf("2: Left child %d of %d has level %u, not %u\n",
                   aatree_key(left), key, (unsigned)llevel,
                   (unsigned)(level-1));
    }
    /* 3. The level of very right child is equal to, or one less than, that
       of its parent */
    if (right != NULL)
    {
        uint32_t rlevel = aatree_level(right);

        if (rlevel != level && rlevel != level-1)
            printf("3: Right child %d of %d has level %u, not %u or %u\n",
                   aatree_key(right), key, (unsigned)rlevel,
                   (unsigned)level, (unsigned)(level-1));
        /* 4. The level of every right grandchild is strictly less than that
           of its grandparent */
        aatree_t gright = aatree_right(right);

        if (gright != NULL)
        {
            uint32_t glevel = aatree_level(gright);

            if (glevel >= level)
                printf("4: Right grandchild %d of %d has level %u, not < %u\n",
                       aatree_key(gright), key, (unsigned)glevel,
                       (unsigned)level);
        }
    }
    /* 5. Every node of level greater than one has two children */
    if (level > 1 && (left == NULL || right == NULL))
        printf("5: Node %d with level %u has one or none children\n",
               key, (unsigned)level);
}

int
main(int argc, char **argv)
{
    int c, delkey, findkey;
    bool delete = false, find = false;
    aatree_t root = NULL;

    opterr = 0;
    while ((c = getopt(argc, argv, "d:f:")) != EOF)
        switch (c)
        {
        case 'd':
            delete = true;
            delkey = atoi(optarg);
            break;
        case 'f':
            find = true;
            findkey = atoi(optarg);
            break;
        default:
            fprintf(stderr, "aatree-test [-d <key>] [-f <key] keys...\n");
            exit(1);
        }
    for (int i = optind ; i < argc ; i++)
    {
        int key = atoi(argv[i]);

        root = aatree_insert(root, key);
        ptree(root, 0);
        aatree_each(root, cnode);
        printf("--------------------\n");
    }
    printf("Order:");
    aatree_each(root, pnode);
    printf("\n--------------------\n");

    if (find)
    {
        printf("Find: %d\n", findkey);
        if (aatree_search(root, findkey))
            printf("  Found\n");
        else
            printf("  Not found\n");
        printf("--------------------\n");
    }
    if (delete)
    {
        printf("Deleting: %d\n", delkey);
        printf("--------------------\n");
        root = aatree_delete(root, delkey);
        aatree_each(root, cnode);
        ptree(root, 0);
        printf("--------------------\n");
        printf("Order:");
        aatree_each(root, pnode);
        printf("\n--------------------\n");
    }

    aatree_destroy(root);

    exit(0);
}
