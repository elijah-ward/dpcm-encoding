#include "dpcm_encoding_function.h"

/**
 * @brief      { validates arguments }
 *
 * @param[in]  argc  The argc
 *
 * @return     { status }
 */

int validate_args(int argc) {
    if (argc != 3) {
        printf("Usage: ./dpcm_encoding [input pgm image filename] [prediction rule code]\nError on argument validation. Exiting...\n");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {

    int status = validate_args(argc);
    if (status == 1) {
        exit(0);
    }

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    float avg_absolute_error, std_absolute_error;
    char *input_pgm_filename = argv[1];
    int prediction_rule = atoi(argv[2]);

    encode_using_dpcm(input_pgm_filename, prediction_rule, &avg_absolute_error, &std_absolute_error);

    end = clock();
    cpu_time_used = ((double) (end - start) / CLOCKS_PER_SEC);

    printf("Encoding Complete!\nElapsed time: %lf\nprediction rule code: %i\navg_absolute_error: %f\nstd_absolute_error: %f\n",
        cpu_time_used, prediction_rule, avg_absolute_error, std_absolute_error);

}