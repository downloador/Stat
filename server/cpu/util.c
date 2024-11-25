#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

char *get_stat_line(int lineNumber)
{
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        printf("Failed to open file\n");
        exit(1);
    }

    char *line = NULL;
    size_t length = 0;
    ssize_t read;

    for (int i = 0; i < lineNumber + 1; i++)
    {
        read = getline(&line, &length, fp);
        if (read == -1)
        {
            printf("Failed to read\n");
            exit(1);
        }
    }

    fclose(fp);

    return line;
}

float get_cpu_util(int core)
{

    int idle = 0;
    int work = 0;

    char *line = get_stat_line(core);

    char *token = strtok(line, " ");
    int index = 0;
    while (token != NULL)
    {
        if (index == 1)
        {
            idle += atoi(token);
        }
        if (index >= 2 && index <= 10)
        {
            work += atoi(token);
        }

        token = strtok(NULL, " ");
        index++;
    }

    float usage = (float) idle * 100 / (float) work;

    free(line);

    return usage;
}