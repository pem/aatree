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
        ptree(root, 0);
        printf("--------------------\n");
        printf("Order:");
        aatree_each(root, pnode);
        printf("\n--------------------\n");
    }

    aatree_destroy(root);

    exit(0);
}
