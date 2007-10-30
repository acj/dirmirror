/*
 * dirmirror: A program to mirror one directory structure to another location.
 *
 * Copyright (c) 2006-2007 Adam Jensen <ajensen@linuxguy.org>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#include "dirmirror.h"

int mirror(char *source_dir, char *target_dir)
{
    DIR *source_dp = opendir(source_dir);
    DIR *target_dp = opendir(target_dir);

    if (source_dp == NULL)
    {
        printf("Couldn't open the directory %s\n", source_dir);
        perror("opendir");
        return -1;
    }
    else if (target_dp == NULL)
    {
        if (mkdir(target_dir, (S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
        {
            printf("Unable to create target directory `%s'\n", target_dir);
            perror("mkdir");
            return -1;
        }
    }

    /* Begin main loop */
    struct dirent *entryp;
    while (entryp = readdir(source_dp))
    {
        if (entryp->d_type == DT_DIR
            && strcmp(entryp->d_name, "..") != 0
            && strcmp(entryp->d_name, ".") != 0)
        {
            /* Assemble next target path and recurse */
            char *next_source 
                = malloc(sizeof(source_dir) + sizeof(entryp->d_name) + 1);
            char *next_target 
                = malloc(sizeof(target_dir) + sizeof(entryp->d_name) + 1);

            if (next_source == NULL || next_target == NULL)
            {
                printf("Out of memory.\n");
                exit(1);
            }

            memset(next_source, 0, sizeof(next_source));
            memset(next_target, 0, sizeof(next_target));
            
            strcpy(next_source, source_dir);
            strcpy(next_target, target_dir);

            strcat(next_source, "/");
            strcat(next_target, "/");

            strcat(next_source, entryp->d_name);
            strcat(next_target, entryp->d_name);

            printf("%s\n", next_source);

            if (mkdir(next_target, (S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
            {
                printf("Unable to create directory: %s (errno: %d)\n", next_target, errno);
                return -1;
            }

            mirror(next_source, next_target); 
        }
        else
        {
            /* Skip the directory/file */
        }
    }

    closedir(source_dp);
    return 0;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
		exit(1);
	}

    printf("Mirroring directory %s to %s\n", argv[1], argv[2]);
    mirror(argv[1], argv[2]);
    return 0;
}
