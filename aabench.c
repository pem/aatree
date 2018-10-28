/*
** Per-Erik Martin (pem@pem.nu) 2018-10-28
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "aatree.h"

#define STV2DOUBLE(T) ((T)->tv_sec + (T)->tv_usec/1000000.0)
#define STVDIFF(T1, T0) (STV2DOUBLE(T1) - STV2DOUBLE(T0))

static void
print_time(const char *s, struct timeval *t0, struct timeval *t1)
{
    printf("%s: %4.3f ms\n", s, 1000*STVDIFF(t1, t0));
}

int
main(int argc, char **argv)
{
    size_t i, count;
    struct timeval t0, t1;
    bool mvariant = false;
    char buf[128];
    char **a = NULL;
    size_t size = 0;
    aatree_t t = NULL;

    if (argc == 2 && strcmp(argv[1], "-m") == 0)
        mvariant = true;

    count = 0;
    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        size_t len = strlen(buf);

        while (len > 0 && buf[len-1] < ' ')
            buf[--len] = '\0';
        if (len == 0)
            continue;           /* Empty line */
        if (count >= size)
        {
            size += 100;
            a = realloc(a, size * sizeof(char *));
            if (!a)
            {
                fprintf(stderr, "realloc(a, %lu) failed\n",
                        (unsigned long)size * sizeof(char *));
                exit(1);
            }
        }
        a[count] = strdup(buf);
        if (!a[count])
        {
            fprintf(stderr, "strdup(\"%s\") failed\n", buf);
            exit(1);
        }
        count += 1;
    }

    gettimeofday(&t0, NULL);
    for (i = 0 ; i < count ; i++)
        if (mvariant)
        {
            if ((t = aatreem_insert(t, a[i], (void *)i)) == NULL)
            {
                fprintf(stderr, "aatreem_insert(t, \"%s\", %lu) failed\n",
                        a[i], (unsigned long)i);
                exit(1);
            }
        }
        else
        {
            aatree_t n = malloc(aatree_sizeof());

            aatree_init_node(n, a[i], (void *)i);
            if ((t = aatree_insert_node(t, n)) == NULL)
            {
                fprintf(stderr, "aatree_insert_node(t, \"%s\", %lu) failed\n",
                        a[i], (unsigned long)i);
                exit(1);
            }
        }
    gettimeofday(&t1, NULL);
    print_time("Insert:", &t0, &t1);

    i = count;
    gettimeofday(&t0, NULL);
    while (i--)
    {
        aatree_t n;

        if ((n = aatree_find_key(t, a[i])) == NULL)
            printf("FIND: No \"%s\" found\n", a[i]);
    }
    gettimeofday(&t1, NULL);
    print_time("Find:  ", &t0, &t1);

    printf("Root level: %u\n", (unsigned)aatree_level(t));

    i = count;
    gettimeofday(&t0, NULL);
    while (i--)
        if (mvariant)
        {
            bool removed = false;

            t = aatreem_delete(t, a[i], &removed, NULL);
            if (! removed)
                printf("REM: No \"%s\" found\n", a[i]);
        }
        else
        {
            aatree_t n = NULL;

            t = aatree_remove_node(t, a[i], &n);
            if (n == NULL)
                printf("REM: No \"%s\" found\n", a[i]);
            else
                free(n);
        }
    gettimeofday(&t1, NULL);
    print_time("Delete:", &t0, &t1);

    if (t != NULL)
    {
        printf("Tree is not empty; destroying...\n");
        if (mvariant)
            aatreem_destroy(t, NULL);
        else
        {
            aatree_t n = NULL;

            while ((t = aatree_remove_node(t, aatree_key(t), &n)) != NULL)
                free(n);
        }
    }

    for (i = 0 ; i < count ; i++)
        free(a[i]);
    free(a);

    exit(0);
}
