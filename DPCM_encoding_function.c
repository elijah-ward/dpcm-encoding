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

    /* ============= INPUT Image Read ============= */

    struct PGM_Image input_image;
    load_PGM_Image(&input_image, in_PGM_filename_ptr);
    int width = input_image.width;
    int height = input_image.height;
    int max_gray = input_image.maxGrayValue;
    int size = width * height;
    int *errors = calloc(size, sizeof(int));
    int *t_values = calloc(size, sizeof(int));

    /* ============ ENCODE DATA ================== */

    for ( int row = 0; row < height; row++ ) {
        for ( int col = 0; col < width; col++ ) {

            int error = 0;
            int prediction;
            int current_px = input_image.image[row][col];

            if ( row == 0 && col == 0 ) {
                // Predict upper left corner as 128 always
                prediction = 128;
                error = current_px - prediction;
                errors[(row*width) + col] = error;
            } else if (row == 0) {
                // Always predict all elements of the first row as West
                prediction = input_image.image[row][col - 1];
                error = current_px - prediction;
                errors[(row*width)+col] = error;
            } else if (row == 1 || col == 0 || col == 1 || col == (width - 1)) {
                // Always predict elements of first col, last col, second row as N
                prediction = input_image.image[row - 1][col];
                error = current_px - prediction;
                errors[(row*width)+col] = error;
            } else {
                // Else we predict using the given prediction rule

                if (prediction_rule == 1) {
                    // always predict West
                    prediction = input_image.image[row][col - 1];
                    error = current_px - prediction;
                    errors[(row*width)+col] = error;
                }

                if (prediction_rule == 2) {
                    // always predict North
                    prediction = input_image.image[row - 1][col];
                    error = current_px - prediction;
                    errors[(row*width)+col] = error;
                }

                if (prediction_rule == 3) {
                    // always predict average of North and West
                    int n = input_image.image[row - 1][col];
                    int w = input_image.image[row][col - 1];
                    prediction = (n + w) / 2;
                    error = current_px - prediction;
                    errors[(row*width)+col] = error;
                }

                if (prediction_rule == 4) {
                    // Use CALIC with GAP
                    int t, prediction, error;

                    // Gather all 7 neightbour pixels
                    int ww = input_image.image[row][col - 2];
                    int w = input_image.image[row][col - 1];
                    int nw = input_image.image[row - 1][col - 1];
                    int n = input_image.image[row - 1][col];
                    int nn = input_image.image[row - 2][col];
                    int ne = input_image.image[row - 1][col + 1];
                    int nne = input_image.image[row - 2][col + 1];

                    int neighbours[7] = {ww,w,nw,n,nn,ne,nne};
                    int n_unique = count_unique(neighbours, 7);
                    // If we have 2 unique, enter binary mode
                    if (n_unique == 2) {
                        // If current pixel == s0 (west pixel)
                        if ( current_px == w ) {
                            t = 0;
                        } else {
                            // Prepare to enter continuous mode in case current_px != s0 or s1
                            t = 2;
                            // If we find that current pixel == s1, send t=1
                            for (int i = 0; i < 7; i++)
                                if (current_px == neighbours[i])
                                    t = 1;
                        }

                        error = 0;
                    }
                    // If don't have 2 distinct values or we have triggered continuous mode from the last block,
                    // Enter continuous mode with GAP calculation
                    if ( n_unique != 2 || t == 2 ) {

                        t = 2;

                        int dh = abs(w - ww) + abs(n - nw) + abs(ne - n);
                        int dv = abs(w - nw) + abs(n - nn) + abs(ne - nne);

                        if (dv - dh > 80) {
                            // sharp horizontal edge
                            prediction = w;
                        } else if (dh - dv > 80) {
                            // sharp vertical edge
                            prediction = n;
                        } else {
                            prediction = ((w + n)/2) + ((ne - nw)/4);
                            if (dv - dh > 32) {
                                // horizontal edge
                                prediction = (0.5 * prediction) + (0.5 * w);
                            } else if (dh - dv > 32) {
                                // vertical edge
                                prediction = (0.5 * prediction) + (0.5 * n);
                            } else if (dv - dh > 8) {
                                // weak horizontal edge
                                prediction = (0.75 * prediction) + (0.25 * w);
                            } else if (dh - dv > 8) {
                                // weak vertical edge
                                prediction = (0.75 * prediction) + (0.25 * n);
                            }
                        }

                        error = current_px - prediction;
                    }
                    t_values[(row*width)+col] = t;
                    errors[(row*width)+col] = error;
                }
            }
        }
    }

    free_PGM_Image(&input_image);

    /* ============== Collect Statistics ============= */

    int max_abs_error = 0;
    int abs_error_sum = 0;

    for ( int row = 0; row < height; row++ ) {
        for ( int col = 0; col < width; col++ ) {
            int abs_error = abs(errors[(row*width)+col]);
            abs_error_sum += abs_error;
            if (abs_error > max_abs_error) max_abs_error = abs_error;
        }
    }

    float abs_error_mean = (float) abs_error_sum / (float) size;
    float abs_error_accumulator = 0.0;

    for ( int row = 0; row < height; row++ ) {
        for ( int col = 0; col < width; col++ ) {
            float abs_error = (float) abs(errors[(row*width)+col]);
            abs_error_accumulator += powf( (abs_error - abs_error_mean), 2 );
        }
    }

    float abs_error_stddev = sqrt(abs_error_accumulator / (float) size);

    *avg_absolute_error_ptr = abs_error_mean;
    *std_absolute_error_ptr = abs_error_stddev;

    /* ========== Gather Frequencies of each Error value ============== */

    int *absolute_error_frequency = calloc(max_abs_error+1, sizeof(int));
    for ( int row = 0; row < height; row++ ) {
        for ( int col = 0; col < width; col++ ) {
            int abs_error = abs(errors[(row*width)+col]);
            absolute_error_frequency[abs_error]++;
        }
    }

    /* ========== OUTPUT Encoded File =========== */

    char encoded_filename[MAX_OUTPUT_FILENAME_LEN];
    snprintf(encoded_filename, MAX_OUTPUT_FILENAME_LEN, "%s.%d.DPCM", in_PGM_filename_ptr, prediction_rule);

    FILE *out_fp;
    out_fp = fopen(encoded_filename, "w+");
    if (!out_fp) perror("Output file failed to write!"), exit(1);

    fprintf(out_fp, "%d %d %d %d\n", prediction_rule, width, height, max_gray);
    fwrite(errors, sizeof(int), size, out_fp);

    if (prediction_rule == 4) {
        fwrite(t_values, sizeof(int), size, out_fp);
    }

    free(errors);
    free(t_values);
    fclose(out_fp);

    /* ========== OUTPUT Error Frequency File =========== */

    char error_frequency_filename[MAX_OUTPUT_FILENAME_LEN];
    snprintf(error_frequency_filename, MAX_OUTPUT_FILENAME_LEN, "%s.%d.errors.csv", in_PGM_filename_ptr, prediction_rule);

    FILE *out_freq_fp;
    out_freq_fp = fopen(error_frequency_filename, "w+");
    if (!out_freq_fp) perror("Frequency file failed to write!"), exit(1);

    fprintf(out_freq_fp, "prediction_error_value,frequency\n");
    for (int i = 0; i <= max_abs_error; i++) {
        int freq = absolute_error_frequency[i];
        if (freq != 0)
            fprintf(out_freq_fp, "%d,%d\n",i,freq);
    }
    free(absolute_error_frequency);
    fclose(out_freq_fp);

}