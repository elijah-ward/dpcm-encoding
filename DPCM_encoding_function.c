#include "DPCM_encoding_function.h"

#define MAX_OUTPUT_FILENAME_LEN 100

/** PRIVATE FUNCTION
 * @brief      Counts the number of unique elements in an integer array.
 *
 * @param      arr   The arr
 * @param[in]  len   The length
 *
 * @return     Number of unique elements.
 */

int count_unique(int *arr, int len) {
    int unique = 1;
    for (int i = 1; i < len; i++) {
        int j = 0;
        for (j = 0; j < i; j++) {
            if (arr[i] == arr[j])
                break;
        }
        if(i==j) unique++;
    }
    return unique;
}

/**
 * @brief      Encodes a PGM image with the DPCM scheme
 *
 * @param      in_PGM_filename_ptr     In pgm filename pointer
 * @param[in]  prediction_rule         The prediction rule
 * @param      avg_absolute_error_ptr  The average absolute error pointer
 * @param      std_absolute_error_ptr  The standard absolute error pointer
 */

void encode_using_dpcm(
    char *in_PGM_filename_ptr,
    int prediction_rule,
    float *avg_absolute_error_ptr,
    float *std_absolute_error_ptr )
{

    /* ============= INPUT File Read ============= */

    FILE *in_fp;
    long f_size;
    char *data;
    in_fp = fopen(in_PGM_filename_ptr, "r");
    if (!in_fp) perror("Input file failed to open!"), exit(1);
    fseek(in_fp, 0L, SEEK_END);
    f_size = ftell(in_fp);
    rewind(in_fp);

    data = calloc(1, f_size + 1);
    fread(data, f_size, 1, in_fp);
    fclose(in_fp);

    /* ============ ENCODE DATA ================== */



    /* ============== Collect Statistics ============= */


    *avg_absolute_error_ptr = avg_error;
    *std_absolute_error_ptr = std_error;

    /* ========== OUTPUT Encoded File =========== */

    char buff_size[10];
    snprintf(buff_size, 10, "%d", searching_buffer_size);

    char out_name_buff[50];
    strcpy(out_name_buff, in_PGM_filename_ptr);
    char *out_filename = strcat( strcat( strcat(out_name_buff, "."), buff_size), ".lz");

    FILE *out_fp;
    out_fp = fopen(out_filename, "w+");
    if (!out_fp) perror("Output file failed to write!"), exit(1);

    fprintf(out_fp,"%u\n%d\n%li\n", searching_buffer_size, n_tokens, f_size);
    fwrite(offsets, sizeof(int), n_tokens, out_fp);
    fwrite(lengths, sizeof(int), n_tokens, out_fp);
    fwrite(mismatches, sizeof(char), n_tokens, out_fp);

    fclose(out_fp);

}