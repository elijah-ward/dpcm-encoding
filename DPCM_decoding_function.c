#include "dpcm_decoding_function.h"

void decode_using_dpcm(char *in_filename_ptr) {

    /* ============== Encoded FILE Input =============== */

    FILE *in_fp;
    in_fp = fopen(in_compressed_filename_ptr, "r");
    if (!in_fp) perror("Input file failed to open!"), exit(1);

    unsigned int searching_buffer_size = 0;
    int n_tokens = 0;
    long f_size = 0;

    fscanf(in_fp, "%u\n%d\n%li\n", &searching_buffer_size, &n_tokens, &f_size);

    int offsets[n_tokens+1];
    int lengths[n_tokens+1];
    char mismatches[n_tokens+1];

    // Read in offset, length, and mismatch arrays

    fread(offsets, sizeof(int), n_tokens, in_fp);
    fread(lengths, sizeof(int), n_tokens, in_fp);
    fread(mismatches, sizeof(char), n_tokens, in_fp);
    fclose(in_fp);

    /* ============ DECODE =========== */



    /* ========= Decoded FILE Output ============ */

    char *out_filename = strcat( in_compressed_filename_ptr, ".pgm");

    FILE *out_fp;
    out_fp = fopen(out_filename, "w+");
    if (!out_fp) perror("Output file failed to open!"), exit(1);

    fwrite(data, sizeof(char), f_size, out_fp);
    fclose(out_fp);

}