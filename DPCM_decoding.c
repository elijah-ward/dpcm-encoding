#include "DPCM_decoding_function.h"

/**
 * @brief      { validates arguments }
 *
 * @param[in]  argc  The argc
 *
 * @return     { status }
 */

int validate_args(int argc) {
    if (argc != 2) {
        printf("Usage: ./dpcm_decoding [input encoded pgm image filename]\nError on argument validation. Exiting...\n");
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

    char *in_encoded_filename = argv[1];
    decode_using_dpcm(in_encoded_filename);

    end = clock();
    cpu_time_used = ((double) (end - start) / CLOCKS_PER_SEC);

    printf("Decoding Complete!\nElapsed time: %lf\n", cpu_time_used);

}