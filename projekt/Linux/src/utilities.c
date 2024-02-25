#include "psir_utilities.h"
#include <stdio.h>

void print_tuples(tuple_struct tuples[], int size, int stats[]) {
    printf("\033[2]\033[H");

    printf("+\n");
    printf("| Tuple Name | No. of Fields | Field 1 | Field 2 |\n");
    printf("-+\n");

    for (int i = 0; i < size; ++i) {
        if (strlen(tuples[i].tuple_name) > 0) {
            printf("| %*s | %*d |", 18, tuples[i].tuple_name, 13, tuples[i].number_of_fields);
        } else {
            printf(" %*s |", 18, ""); // Placeholder for empty field
        }

        for (int j = 0; j < 2; ++j) {
            if (j < tuples[i].number_of_fields) {
                if (tuples[i].fields[j].is_actual) {
                    if (tuples[i].fields[j].type == 0) {
                        printf("Int: %13d ", tuples[i].fields[j].data.int_field);
                    } else if (tuples[i].fields[j].type == 1) {
                        printf("Float: %11.2f ", tuples[i].fields[j].data.float_field);
                    } else if (tuples[i].fields[j].type == 2) {
                        printf("String: %s ", tuples[i].fields[j].data.string_field);
                    }
                } else {
                    printf(" %*s |", 18, ""); // Placeholder for empty field
                }
            }
        }
        printf("|");
        printf("\n");
    }
    printf("-+\n");
}

