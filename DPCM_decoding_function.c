#include "DPCM_decoding_function.h"

#define MAX_OUTPUT_FILENAME_LEN 100

void decode_using_dpcm(char *in_filename_ptr) {

    /* ============== Encoded FILE Input =============== */

    FILE *in_fp;
    in_fp = fopen(in_filename_ptr, "r");
    if (!in_fp) perror("Input file failed to open!"), exit(1);

    int prediction_rule, width, height, max_gray;
    fscanf(in_fp, "%d %d %d %d\n", &prediction_rule, &width, &height, &max_gray);

    int size = width*height;

    int errors[size];
    fread(errors, sizeof(int), size, in_fp);

    int t_values[size];
    if ( prediction_rule == 4 ) {
        fread(t_values, sizeof(int), size, in_fp);
    }

    fclose(in_fp);

    struct PGM_Image test_image;
    load_PGM_Image(&test_image, "goldhill.raw.pgm");

    struct PGM_Image output_image;
    create_PGM_Image(&output_image, width, height, max_gray);

    /* ============ DECODE DATA ================== */

    for ( int row = 0; row < height; row++ ) {
        for ( int col = 0; col < width; col++ ) {

            int error = errors[(row*width) + col];
            int prediction;
            int current_px;

            // Predict upper left corner as 128 always
            if ( row == 0 && col == 0 ) {
                prediction = 128;
                current_px = error + prediction;
                output_image.image[row][col] = current_px;
            } else if (row == 0) {
                prediction = output_image.image[row][col - 1];
                current_px = error + prediction;
                output_image.image[row][col] = current_px;
            } else if (row == 1 || col == 0 || col == 1 || col == (width - 1)) {
                prediction = output_image.image[row - 1][col];
                current_px = error + prediction;
                output_image.image[row][col] = current_px;
            } else {

                if (prediction_rule == 1) {
                    // always predict West
                    prediction = output_image.image[row][col - 1];
                    current_px = error + prediction;
                    output_image.image[row][col] = current_px;
                }

                if (prediction_rule == 2) {
                    // always predict North
                    prediction = output_image.image[row - 1][col];
                    current_px = error + prediction;
                    output_image.image[row][col] = current_px;
                }

                if (prediction_rule == 3) {
                    // always predict average of North and West
                    int n = output_image.image[row - 1][col];
                    int w = output_image.image[row][col - 1];
                    prediction = (n + w) / 2;
                    current_px = error + prediction;
                    output_image.image[row][col] = current_px;
                }

                if (prediction_rule == 4) {
                    // CALIC

                    int t = t_values[(row*width) + col];

                    int ww = output_image.image[row][col - 2];
                    int w = output_image.image[row][col - 1];
                    int nw = output_image.image[row - 1][col - 1];
                    int n = output_image.image[row - 1][col];
                    int nn = output_image.image[row - 2][col];
                    int ne = output_image.image[row - 1][col + 1];
                    int nne = output_image.image[row - 2][col + 1];

                    int neighbours[7] = {ww,w,nw,n,nn,ne,nne};

                    if (t == 0) {
                        current_px = w;
                    } else if (t == 1) {
                        for (int i = 0; i < 7; i++)
                            if (neighbours[i] != w)
                                current_px = neighbours[i];
                    } else {

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

                        current_px = error + prediction;
                    }
                    output_image.image[row][col] = current_px;
                }
            }
        }
    }


    // /* ========= PGM Image Output ============ */

    char out_filename[MAX_OUTPUT_FILENAME_LEN];
    snprintf(out_filename, MAX_OUTPUT_FILENAME_LEN, "%s.pgm", in_filename_ptr);
    save_PGM_Image(&output_image, out_filename, 1);
    free_PGM_Image(&output_image);
}