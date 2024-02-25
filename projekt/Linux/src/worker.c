#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "tuple.h"
#include <time.h>

#define REMAINDER 0
#define ID 1

int main() {
    field_t template[2];
    field_t field[3];
    int factor1, factor2;
    float result;
    setSocket("0.0.0.0", 5012);

    template[0].dataType = TYPE_INT;
    template[1].dataType = TYPE_INT;
    ts_inp(ID, template, 2);

    field[0].dataType = template[0].dataType;
    field[0].data.int_type = template[0].data.int_type;
    field[1].dataType = template[1].dataType;
    field[1].data.int_type = template[1].data.int_type;

    factor1 = field[0].data.int_type;
    factor2 = field[1].data.int_type;
    result = (float)factor1/factor2;

    printf("\nResult: %f", result);

    field[2].dataType = TYPE_FLOAT;
    field[2].data.float_type = result;

    ts_out(ID, field, 3);


    return 0;
}