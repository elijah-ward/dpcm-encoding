#include <stdio.h>
#include <stdlib.h>
#include "mean_absolute_error.h"

/**
 * @brief      { validates arguments }
 *
 * @param[in]  argc  The argc
 *
 * @return     { status }
 */

int validate_args(int argc) {
    if (argc != 3) {
        printf("Usage: ./compare_pgm_images [input pgm image filename 1] [input pgm image filename 2]\nError on argument validation. Exiting...\n");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {

    int status = validate_args(argc);
    if (status == 1) {
        exit(0);
    }

    char *input_pgm_filename_1 = argv[1];
    char *input_pgm_filename_2 = argv[2];

    float mae = mean_absolute_error(input_pgm_filename_1, input_pgm_filename_2);
    printf("MAE is: %f\n", mae);


}
