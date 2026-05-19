/* qxcalib.h – QuantXT calibration module */

#ifndef QXCALIB_H
#define QXCALIB_H

#include <stdio.h>

/* Forward‑declare your existing model state type.  
   Change this include/name to match QuantXT. */
#include "state.h"   /* typedef struct QXState QXState; */

typedef struct {
    int   rows;         /* number of precedent rows used      */
    float total_error;  /* sum of squared errors              */
    float mean_error;   /* total_error / rows (or 0 if rows=0)*/
} QXCalibResult;

/* Runs calibration against a precedent file and writes a TXT summary.
 *
 *  precedent_filename : input file with precedent cases
 *  result_filename    : output TXT file with summary
 *  out_result         : optional; can be NULL
 *
 * Returns:
 *   0 on success, non‑zero on error (file, parse, etc.).
 */
int qxcalib_run(const char *precedent_filename,
                 const char *result_filename,
                 QXCalibResult *out_result);

#endif /* QXCALIB_H */
