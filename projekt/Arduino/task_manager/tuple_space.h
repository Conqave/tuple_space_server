#ifndef TUPLE_SPACE_H
#define TUPLE_SPACE_H

#include "tuple.h"

#define TS_OUT 0
#define TS_INP 1
#define TS_RDP 2

#define MAX_ELEMENTS 10
#define MAX_TUPLES 512

typedef struct {
    unsigned int id;
    long num_fields;
    char data_type[MAX_ELEMENTS];
    union {
        long int_type;
        float float_type;
    } data[MAX_ELEMENTS];
} tuple_t;

typedef struct {
    unsigned int id;
    char operation_type;
    char num_fields;
    char data_type;

} protocol_header;




#endif
