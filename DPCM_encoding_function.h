#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "libpnm.h"

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
    float *std_absolute_error_ptr);

