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
    field_t field[2];
    field_t template[3];
    srand(time(0));
    setSocket("0.0.0.0", 5012);

    //while(1) {
        //TS_OUT Test
        field[0].dataType = TYPE_INT;
        field[0].data.int_type = 37;
        field[1].dataType = TYPE_INT;
        field[1].data.int_type = 21;
        ts_out(ID, field, 2);

        usleep(5000000);

        template[0].dataType = TYPE_INT;
        template[1].dataType = TYPE_INT;
        template[2].dataType = TYPE_FLOAT;
        ts_rdp(ID, template, 3);

        printf("\nResult: %f\n", template[2].data.float_type);

        usleep(1000000);
    //}




    return 0;
}
