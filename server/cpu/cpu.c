#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h> 

#include "msr.c"
#include "util.c"
#include "temp.c"

int temperature, wattage;
float packageUtilization, totalUtilization;

void* love_pthread() {
    int w = get_package_power(0, 1);
    return w;
}

void* cpu_collect() {
    int cores = (int) sysconf(_SC_NPROCESSORS_ONLN);

    while (true) {
        temperature = get_die_temp();
        packageUtilization = get_cpu_util(0);

        totalUtilization = 0;
        for (int i = 1; i < cores; i++) {
            totalUtilization += get_cpu_util(i);
        }

        pthread_t ptid;
        pthread_create(&ptid, NULL, &love_pthread, NULL);
        pthread_join(ptid, &wattage);
    }
}

void export_info(int **temp, int **watt, float **packageUtil, float **totalUtil) {
    *temp = &temperature;
    *watt = &wattage;
    *packageUtil = &packageUtilization;
    *totalUtil = &totalUtilization;
}