/* qxcalib.c – QuantXT calibration module */

#include "qxcalib.h"
#include "state.h"
#include "engine.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------
   INTEGRATION POINTS – REAL QUANTXT ENGINE HOOKS
   ------------------------------------------------------------------ */

/* Your real evaluation function from engine.c */
extern float run_engine(const State *st);

/* Map key=value into your real State struct */
static int qxcalib_assign_field(State *st,
                                const char *key,
                                const char *value)
{
    if      (strcmp(key, "int_rev")            == 0) st->int_rev            = (float)atof(value);
    else if (strcmp(key, "debt_gdp")           == 0) st->debt_gdp           = (float)atof(value);
    else if (strcmp(key, "sahm")               == 0) st->sahm               = (float)atof(value);
    else if (strcmp(key, "infl")               == 0) st->infl               = (float)atof(value);
    else if (strcmp(key, "unemp")              == 0) st->unemp              = (float)atof(value);
    else if (strcmp(key, "gdp")                == 0) st->gdp                = (float)atof(value);
    else if (strcmp(key, "tail_risk")          == 0) st->tail_risk          = (float)atof(value);
    else if (strcmp(key, "liq_gap")            == 0) st->liq_gap            = (float)atof(value);
    else if (strcmp(key, "usd_reserve_share")  == 0) st->usd_reserve_share  = (float)atof(value);
    else if (strcmp(key, "cbo_deficit")        == 0) st->cbo_deficit        = (float)atof(value);
    else if (strcmp(key, "xdate")              == 0) st->xdate              = (float)atof(value);
    else if (strcmp(key, "ofr")                == 0) st->ofr                = (float)atof(value);
    else if (strcmp(key, "hy_spread")          == 0) st->hy_spread          = (float)atof(value);
    else if (strcmp(key, "dxy_mom")            == 0) st->dxy_mom            = (float)atof(value);
    else if (strcmp(key, "oil_price")          == 0) st->oil_price          = (float)atof(value);
    else if (strcmp(key, "ai_capex")           == 0) st->ai_capex           = (float)atof(value);
    else if (strcmp(key, "lagged_ai")          == 0) st->lagged_ai          = (float)atof(value);
    else if (strcmp(key, "geopolitical_risk")  == 0) st->geopolitical_risk  = (float)atof(value);
    else if (strcmp(key, "investor_sentiment") == 0) st->investor_sentiment = (float)atof(value);
    else return 1; /* unknown field */

    return 0;
}

/* ------------------------------------------------------------------
   INTERNAL PARSING HELPERS
   ------------------------------------------------------------------ */

#define QXCALIB_MAX_ROWS  64
#define QXCALIB_LINE_LEN 256

typedef struct {
    State state;
    float target;
    int   has_target;
} QXCalibRow;

/* Trim whitespace */
static char *qxcalib_trim(char *s)
{
    char *end;

    while (*s==' '||*s=='\t'||*s=='\r'||*s=='\n')
        s++;

    if (*s == '\0')
        return s;

    end = s + strlen(s) - 1;
    while (end > s &&
          (*end==' '||*end=='\t'||*end=='\r'||*end=='\n'))
        *end-- = '\0';

    return s;
}

/* Parse key=value */
static int qxcalib_parse_kv(char *line, char **out_key, char **out_val)
{
    char *eq = strchr(line, '=');
    if (!eq)
        return 1;

    *eq = '\0';
    *out_key = qxcalib_trim(line);
    *out_val = qxcalib_trim(eq + 1);

    if (**out_key == '\0' || **out_val == '\0')
        return 1;

    return 0;
}

/* ------------------------------------------------------------------
   PRECEDENT FILE FORMAT
   ------------------------------------------------------------------ */

static void qxcalib_finalize_row(QXCalibRow *rows,
                                 int *row_count,
                                 QXCalibRow *current)
{
    if (!current->has_target)
        return;

    if (*row_count >= QXCALIB_MAX_ROWS)
        return;

    rows[*row_count] = *current;
    (*row_count)++;
}

static int qxcalib_load_precedent(const char *filename,
                                  QXCalibRow *rows,
                                  int max_rows)
{
    FILE *fp;
    char  line[QXCALIB_LINE_LEN];
    char *p;
    char *key, *val;        /* <-- moved up here */
    QXCalibRow cur;
    int count = 0;

    (void)max_rows;

    fp = fopen(filename, "r");
    if (!fp)
        return -1;

    memset(&cur, 0, sizeof(cur));

    while (fgets(line, sizeof(line), fp)) {
        p = qxcalib_trim(line);     /* assignment, not declaration */

        if (*p == '\0') {
            qxcalib_finalize_row(rows, &count, &cur);
            memset(&cur, 0, sizeof(cur));
            continue;
        }

        if (*p == '#' || *p == ';')
            continue;

        if (qxcalib_parse_kv(p, &key, &val) != 0)
            continue;

        if (strcmp(key, "target") == 0) {
            cur.target = (float)atof(val);
            cur.has_target = 1;
        } else {
            qxcalib_assign_field(&cur.state, key, val);
        }
    }

    qxcalib_finalize_row(rows, &count, &cur);

    fclose(fp);
    return count;
}
/* ------------------------------------------------------------------
   CORE CALIBRATION LOOP
   ------------------------------------------------------------------ */

static void qxcalib_compute(const QXCalibRow *rows,
                            int count,
                            QXCalibResult *out)
{
    float total_err = 0.0f;
    int   i;

    for (i = 0; i < count; i++) {
        float model = run_engine(&rows[i].state);
        float diff  = model - rows[i].target;
        total_err  += diff * diff;
    }

    out->rows        = count;
    out->total_error = total_err;
    out->mean_error  = (count > 0 ? total_err / (float)count : 0.0f);
}

/* ------------------------------------------------------------------
   RESULT WRITER
   ------------------------------------------------------------------ */

static int qxcalib_write_result(const char *filename,
                                const QXCalibResult *r)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
        return -1;

    fprintf(fp, "QUANTXT CALIBRATION RESULT\n");
    fprintf(fp, "--------------------------\n");
    fprintf(fp, "Rows tested : %d\n", r->rows);
    fprintf(fp, "Total error : %.6f\n", r->total_error);
    fprintf(fp, "Mean error  : %.6f\n", r->mean_error);

    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------
   PUBLIC ENTRY POINT
   ------------------------------------------------------------------ */

int qxcalib_run(const char *precedent_filename,
                const char *result_filename,
                QXCalibResult *out_result)
{
    static QXCalibRow rows[QXCALIB_MAX_ROWS];
    QXCalibResult res;

    int n = qxcalib_load_precedent(precedent_filename,
                                   rows,
                                   QXCALIB_MAX_ROWS);

    if (n < 0)
        return 1;

    qxcalib_compute(rows, n, &res);

    if (qxcalib_write_result(result_filename, &res) != 0)
        return 2;

    if (out_result)
        *out_result = res;

    return 0;
}
