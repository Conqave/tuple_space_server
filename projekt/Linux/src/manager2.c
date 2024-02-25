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
    field_t field[2];
    setSocket("0.0.0.0", 5012);
    //while(1) {
        field[0].dataType = TYPE_FLOAT;
        field[0].data.float_type = 18.75;
        field[1].dataType = TYPE_FLOAT;
        field[1].data.float_type = 0.32;
        ts_out(ID, field, 2);

        usleep(1000000);


    //}

    /*ts_inp(ID, template, 2);

    template[0].dataType = TYPE_INT;
    template[1].dataType = TYPE_FLOAT;
    printf("Received Data From Worker: %f", template[1].data.float_type);*/


    return 0;
}