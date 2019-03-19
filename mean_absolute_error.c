#include "mean_absolute_error.h"

/**
 * @brief      { calculates the mean absolute error of two images }
 *
 * @param      filename_1_ptr  The filename 1 pointer
 * @param      filename_2_ptr  The filename 2 pointer
 *
 * @return     { (float) the mean absolute error of the two images }
 */

float mean_absolute_error(char *filename_1_ptr, char *filename_2_ptr) {

    // Initialize values
    struct PGM_Image pgm_1;
    load_PGM_Image(&pgm_1, filename_1_ptr);
    int height_1 = pgm_1.height;
    int width_1 = pgm_1.width;
    int max_gray_1 = pgm_1.maxGrayValue;

    struct PGM_Image pgm_2;
    load_PGM_Image(&pgm_2, filename_2_ptr);
    int height_2 = pgm_2.height;
    int width_2 = pgm_2.width;
    int max_gray_2 = pgm_2.maxGrayValue;

    // If shape or size is not equal, return -1
    if ( (height_1 != height_2) || (width_1 != width_2) ) {
        printf("ERROR: Two images of unequal size / shape passed as input... Returning -1.\n");
        return (float) -1;
    }

    // If max gray values are different, normalize each value in the larger value image to the lesser value
    if ( max_gray_1 != max_gray_2 ) {
        int lesserGray, greaterGray;
        struct PGM_Image *greaterImage;

        if ( max_gray_1 < max_gray_2 ) {
            *greaterImage = pgm_2;
            lesserGray = max_gray_1;
            greaterGray = max_gray_2;
        } else {
            *greaterImage = pgm_1;
            lesserGray = max_gray_2;
            greaterGray = max_gray_1;
        }

        for ( int row = 0; row < greaterImage->height; row++ ) {
            for ( int col = 0; col < greaterImage->width; col++ ) {
                int currentVal = greaterImage->image[row][col];
                int adjustedVal = (currentVal / greaterGray) * lesserGray;
                greaterImage->image[row][col] = adjustedVal;
            }
        }
    }

    // Calculate the absolute difference between each respective pixel in the images

    float difference_sum = 0;

    for ( int row = 0; row < pgm_1.height; row++ ) {
        for ( int col = 0; col < pgm_1.width; col++ ) {
            int val1 = pgm_1.image[row][col];
            int val2 = pgm_2.image[row][col];
            difference_sum += (float) abs(val1 - val2);
        }
    }

    float mae = difference_sum / ((float) (height_1 * width_1));
    return mae;
}