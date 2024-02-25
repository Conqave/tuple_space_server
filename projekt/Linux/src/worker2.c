#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "tuple.h"
#include <time.h>

#define REMAINDER 0
#define ID 2

int main() {
    field_t template[2];
    setSocket("0.0.0.0", 5012);

    template[0].dataType = TYPE_FLOAT;
    template[1].dataType = TYPE_FLOAT;
    ts_inp(ID, template, 2);

    printf("\nTemperature: %f\n", template[0].data.float_type);
    printf("Humidity: %f\n", template[0].data.float_type);

    usleep(3000000);

    return 0;
}