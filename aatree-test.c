/*
** Per-Erik Martin (pem@pem.nu) 2018-10-15
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "aatreem.h"

#define UNUSED(x) ((void)(x))

typedef struct taatree_s
{
    aatree_t base;              /* Must be first */
    char *condval;
} taatree_t;

static void
ptree(aatree_node_t *n, int indent)
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
pnode(aatree_t *t, aatree_node_t *n)
{
    UNUSED(t);
    char *val = aatree_value(n);

    if (val == NULL)
        printf(" %s", aatree_key(n));
    else
        printf(" %s:%s", aatree_key(n), val);
    return true;
}

static uint32_t AMcount = 0;

static bool
abortminus(aatree_t *t, aatree_node_t *n)
{
    UNUSED(t);
    char *val = aatree_value(n);

    if (val != NULL && val[0] == '-')
    {
        printf("Found %s:%s at %u - Aborting!\n",
               aatree_key(n), val, (unsigned)AMcount);
        return false;
    }
    AMcount += 1;
    return true;
}

static bool
condval_check(aatree_t *t, aatree_node_t *n)
{
    taatree_t *root = (taatree_t *)t;
    char *val = aatree_value(n);

    return (strcmp(val, root->condval) == 0);
}

/* Check invariants for AA Trees */
static bool
cnode(aatree_t *t, aatree_node_t *n)
{
    UNUSED(t);
    char *key = aatree_key(n);
    aatree_level_t level = aatree_level(n);
    aatree_node_t *left = aatree_left(n);
    aatree_node_t *right = aatree_right(n);

    /* 1. The level of every leaf node is one */
    if (left == NULL && right == NULL && level != 1)
        printf("1: Leaf node %s has level %u\n", key, (unsigned)level);
    /* 2. The level of every left child is exactly one less than that of
       its parent */
    if (left != NULL)
    {
        aatree_level_t llevel = aatree_level(left);

        if (llevel != level-1)
            printf("2: Left child %s of %s has level %u, not %u\n",
                   aatree_key(left), key, (unsigned)llevel,
                   (unsigned)(level-1));
    }
    /* 3. The level of very right child is equal to, or one less than, that
       of its parent */
    if (right != NULL)
    {
        aatree_level_t rlevel = aatree_level(right);

        if (rlevel != level && rlevel != level-1)
            printf("3: Right child %s of %s has level %u, not %u or %u\n",
                   aatree_key(right), key, (unsigned)rlevel,
                   (unsigned)level, (unsigned)(level-1));
        /* 4. The level of every right grandchild is strictly less than that
           of its grandparent */
        aatree_node_t *gright = aatree_right(right);

        if (gright != NULL)
        {
            aatree_level_t glevel = aatree_level(gright);

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

static void
usage(void)
{
    fprintf(stderr, "aatree-test [-D|-r|-u|-R old/new] [-v] [-d <key>[:<val>]] [-f <key>[:<val>]] keys...\n");
    exit(1);
}

int
main(int argc, char **argv)
{
    int c;
    char *delkey, *findkey, *oldkey = NULL, *newkey = NULL;
    bool verbose = false, delete = false, find = false, unique = false,
        replace = false, rename = false, height = false;
    uint32_t count = 0;
    taatree_t *root = NULL;

    opterr = 0;
    while ((c = getopt(argc, argv, "HR:d:f:ruv")) != EOF)
        switch (c)
        {
        case 'H':
            height = true;
            break;
        case 'R':
            rename = true;
            oldkey = optarg;
            break;
        case 'd':
            delete = true;
            delkey = strdup(optarg);
            break;
        case 'f':
            find = true;
            findkey = strdup(optarg);
            break;
        case 'r':
            replace = true;
            break;
        case 'u':
            unique = true;
            break;
        case 'v':
            verbose = true;
            break;
        default:
            usage();
        }
    if ((replace && unique) || (replace && rename) || (unique && rename))
        usage();

    root = (taatree_t *)aatreem_create(sizeof(taatree_t));

    if (rename)
    {
        oldkey = strdup(oldkey);
        newkey = strchr(oldkey, '/');
        if (newkey == NULL)
        {
            free(oldkey);
            usage();
        }
        *newkey++ = '\0';
        if (*oldkey == '\0' || *newkey == '\0')
        {
            free(oldkey);
            usage();
        }
    }
    for (int i = optind ; i < argc ; i++)
    {
        char *key = strdup(argv[i]);
        char *val = strchr(key, ':');

        if (val != NULL)
        {
            *val++ = '\0';
            val = strdup(val);
        }
        if (!replace && !unique)
        {
            aatreem_insert(&root->base, key, val);
            count += 1;
        }
        else if (replace)
        {
            void *oldval = NULL;

            aatreem_replace(&root->base, key, val, &oldval);
            if (oldval == NULL)
                count += 1;
            else
            {
                printf("Replaced %s, old value is %s\n",
                       key, (oldval != NULL ? (char *)oldval : "(null)"));
                free(oldval);
            }
        }
        else
        {
            void *xists = NULL;

            if (! aatreem_insert_unique(&root->base, key, val, &xists))
            {
                printf("Key is not unique:");
                printf(" %s", key);
                if (xists != NULL)
                    printf(":%s", (char *)xists);
                putchar('\n');
                free(val);
            }
            count += 1;
        }
        free(key);
        if (verbose)
        {
            ptree(root->base.root, 0);
            printf("--------------------\n");
        }
        if (! aatree_each(&root->base, cnode))
            printf("aatree_each cnode returned false\n");
    }
    if (! verbose)
    {
        ptree(root->base.root, 0);
        printf("--------------------\n");
    }
    if (height)
    {
        printf("Count: %lu (log2: %g)\n",
               (unsigned long)count, (count == 0 ? 0 : ceil(log2(count+1))));
        printf("Height: %lu\n", (unsigned long)aatree_height(&root->base));
        printf("--------------------\n");
    }
    for (int i = optind ; i < argc ; i++)
    {
        aatree_node_t *n;
        char *key = strdup(argv[i]); /* Because we might write in it */
        char *val = strchr(key, ':');

        if (val != NULL)
            *val++ = '\0';
        n = aatree_find_key(&root->base, key, NULL);
        if (n == NULL)
            printf("Didn't find %s\n", argv[i]);
        free(key);
    }
    printf("Each:");
    if (! aatree_each(&root->base, pnode))
        printf("aatree_each pnode returned false\n");
    printf("\n--------------------\n");
    printf("Iter:");
    {
        aatree_iter_t iter;
        aatree_node_t *n;
        if (! aatree_iter_init(&root->base, &iter))
            fprintf(stderr, "Tree is too deep for iterator\n");
        else
            while ((n = aatree_iter_next(&iter)) != NULL)
                (void)pnode(&root->base, n);
    }
    printf("\n--------------------\n");

    if (find)
    {
        aatree_node_t *n;
        aatree_iter_t iter;
        char *condval;
        aatree_condition_fun_t *cond = NULL;

        printf("Find: %s\n", findkey);
        if ((condval = strchr(findkey, ':')) != NULL)
        {
            *condval++ = '\0';
            root->condval = condval;
            cond = condval_check;
        }
        if ((n = aatree_find_key(&root->base, findkey, cond)) == NULL)
            printf("  Not found\n");
        else
        {
            char *val = aatree_value(n);

            printf("  Found %s\n", (val == NULL ? "(null)" : val));
        }
        printf("--------------------\n");
        if (condval == NULL)
            printf("Iter find: %s\n", findkey);
        else
            printf("Iter find: %s:%s\n", findkey, condval);
        if (! aatree_iter_key_init(&root->base, findkey, &iter))
            fprintf(stderr, "Tree is too deep for iterator\n");
        else
        {
            bool pfound = true;

            while ((n = aatree_iter_key_next(&iter)) != NULL)
            {
                char *val = aatree_value(n);

                if (condval != NULL &&
                    strcmp(val, condval) != 0)
                    continue;
                if (pfound)
                {
                    printf("  Found");
                    pfound = false;
                }
                printf(" %s", (val == NULL ? "(null)" : val));
            }
            if (pfound)
                printf("  Iter not found");
            putchar('\n');
        }
        printf("--------------------\n");
        free(findkey);
    }
    if (delete)
    {
        char *delval = NULL;
        char *condval;
        aatree_condition_fun_t *cond = NULL;

        printf("Deleting: %s\n", delkey);
        if ((condval = strchr(delkey, ':')) != NULL)
        {
            *condval++ = '\0';
            root->condval = condval;
            cond = condval_check;
        }
        if (! aatreem_delete(&root->base, delkey, cond, (void *)&delval))
            printf("  Not deleted\n");
        else
        {
            free(delval);
            printf("  Deleted\n");
        }
        if (! aatree_each(&root->base, cnode))
            printf("aatree_each cnode returned false\n");
        ptree(root->base.root, 0);
        printf("--------------------\n");
        printf("Order:");
        if (! aatree_each(&root->base, pnode))
            printf("aatree_each pnode returned false\n");
        printf("\n--------------------\n");
        free(delkey);
    }
    if (rename)
    {
        printf("Renaming: %s -> %s\n", oldkey, newkey);
        aatreem_rename(&root->base, oldkey, newkey);
        if (! aatree_each(&root->base, cnode))
            printf("aatree_each cnode returned false\n");
        ptree(root->base.root, 0);
        printf("--------------------\n");
        printf("Order:");
        if (! aatree_each(&root->base, pnode))
            printf("aatree_each pnode returned false\n");
        printf("\n--------------------\n");
        free(oldkey);
    }

    if (! aatree_each(&root->base, abortminus))
        printf("Aborted on minus\n");

    aatreem_destroy(&root->base, free);

    exit(0);
}
