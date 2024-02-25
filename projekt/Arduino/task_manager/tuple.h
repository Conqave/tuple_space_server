#ifndef TUPLE_H
#define TUPLE_H

//status
#define EMPTY 0
#define TO_PROCESS 1
#define READY 2
#define ERROR 3

//Data Type
#define TYPE_INT 0
#define TYPE_FLOAT 1

#define TS_SUCCESS 0
#define TS_FAILURE 1

typedef struct {
    int dataType;
    union {
        long int_type;
        float float_type;
    } data;
} field_t;


int ts_out(int id, field_t* fields, int num_fields);
int ts_inp(int id, field_t* fields, int num_fields);
int ts_rdp(int id, field_t* fields, int num_fields);


#endif
