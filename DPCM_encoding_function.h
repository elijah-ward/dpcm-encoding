#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

void encode_using_dpcm(
    char *in_PGM_filename_ptr,
    int prediction_rule,
    float *avg_absolute_error_ptr,
    float *std_absolute_error_ptr);

