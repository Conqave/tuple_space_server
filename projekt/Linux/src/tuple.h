#ifndef TUPLE_H
#define TUPLE_H

//Data Type
#define TYPE_INT 0
#define TYPE_FLOAT 1

#define TS_SUCCESS 0
#define TS_FAILURE 1

typedef struct {
    int dataType;
    union {
        int int_type;
        float float_type;
    } data;
} field_t;


void setSocket(char* ip, int port);
int ts_out(int id, field_t* fields, int num_fields);
int ts_inp(int id, field_t* fields, int num_fields);
int ts_rdp(int id, field_t* fields, int num_fields);


#endif