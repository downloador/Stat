#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

float get_die_temp() {
    int file_descriptor = open("/sys/class/hwmon/hwmon2/temp2_input", O_RDONLY);
    if (file_descriptor == -1) {
        exit(2);
    }

    char line[6] = {0};
    read(file_descriptor, &line, sizeof line);

    float temp = (float) atoi(line) / 1000;
    return temp;
}