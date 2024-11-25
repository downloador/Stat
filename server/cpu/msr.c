#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>

int open_msr(int core) {
    char msr_filename[BUFSIZ];
    sprintf(msr_filename, "/dev/cpu/%d/msr", core);
    
    int file_descriptor = open(msr_filename, O_RDONLY);
    if (file_descriptor == -1) {
        if (errno == ENXIO) {
            fprintf(stderr, "open-msr: CPU MSR %d not found\n", core);
            exit(2);
        } else if (errno == EIO) {
            fprintf(stderr, "open-msr: CPU MSR %d I/O error, your CPU probably doesn't support MSRs\n", core);
            exit(3);
        } else {
            perror("open-msr");
            fprintf(stderr, "Something went wrong trying to open %s\n", msr_filename);
            exit(127);
        }
    }

    return file_descriptor;
}

static long read_msr(int file_descriptor, unsigned int offset) {
    uint64_t data;

    if (pread(file_descriptor, &data, sizeof data, offset) != sizeof data) {
        perror("read-msr");
        exit(127);
    }

    return (long) data;
}

//

#define AMD_MSR_PWR_UNIT 0xC0010299
#define AMD_MSR_PACKAGE_ENERGY 0xC001029B
#define AMD_ENERGY_UNIT_MASK 0x1F00

int get_package_power(int core, int accuracy) {
    int file_descriptor_core = open_msr(core);

    int core_energy_units = read_msr(file_descriptor_core, AMD_MSR_PWR_UNIT);
    unsigned int energy_unit = (core_energy_units & AMD_ENERGY_UNIT_MASK) >> 8;
    double energy_unit_d = pow(0.5, (double) energy_unit);

    int package_raw = read_msr(file_descriptor_core, AMD_MSR_PACKAGE_ENERGY) * energy_unit_d;

    int sleep_duration = pow(10, 3 - log10(accuracy));
    usleep(sleep_duration * 1000); // Sleep for s_d milliseconds, or s_d*1000 microseconds

    int package_raw_delta = read_msr(file_descriptor_core, AMD_MSR_PACKAGE_ENERGY) * energy_unit_d;

    int package_power = (package_raw_delta - package_raw) * pow(10, log10(accuracy));

    return package_power;
}