#include <pthread.h> 

#include "server.c"

#include "cpu/cpu.c"

int main() {
    pthread_t ptid0, ptid1; 

    pthread_create(&ptid0, NULL, &initialize, NULL);
    pthread_create(&ptid1, NULL, &cpu_collect, NULL);

    pthread_join(ptid0, 0);
    pthread_join(ptid1, 0);

    return 0;
}