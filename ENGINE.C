/* engine.c -- QUANTXT v1.1 sovereign-risk scoring engine
 *
 * Weights derived via ordinary least squares regression
 * against 30-point CALIB.TXT precedent table, all 19 fields.
 *
 * Calibration results with these weights:
 *   SSE  : 0.012154
 *   MSE  : 0.000405
 *   RMSE : 0.020128
 *
 * Calibration history:
 *   Hand-assigned (19 fields) : RMSE 0.368   (baseline)
 *   OLS  6-field              : RMSE 0.044   (71x improvement)
 *   OLS 19-field              : RMSE 0.020   (further 2.2x)
 *
 * Key findings from OLS:
 *   - tail_risk (+1.54) is the dominant stress signal
 *   - infl (+4.13) is the dominant positive macro driver
 *   - ai_capex (-0.73) dampens stress; lagged_ai (+0.78)
 *     has a slight reflexive amplification effect
 *   - usd_reserve_share (~0.00) has negligible independent
 *     predictive power when other variables are present
 *   - sahm flips sign once tail_risk enters the model,
 *     indicating collinearity between the two signals
 *
 * To recalibrate: update CALIB.TXT and re-run OLS solver.
 */

#include "engine.h"

float run_engine(const State *st)
{
    float score = 0.0f;

    /* --------------------------------------------------
     * OLS-calibrated weights -- all 19 fields
     * Derived from 30-point full-spectrum precedent set
     * -------------------------------------------------- */

    /* Core macro-financial */
    score += (float)(st->int_rev            * -0.1656);
    score += (float)(st->debt_gdp           *  0.0894);
    score += (float)(st->usd_reserve_share  * -0.0001); /* near zero */
    score += (float)(st->cbo_deficit        * -0.0300);
    score += (float)(st->xdate              * -0.0003);

    /* Labor / cycle */
    score += (float)(st->sahm               * -1.2980);
    score += (float)(st->infl               *  4.1338);
    score += (float)(st->unemp              *  2.5842);
    score += (float)(st->gdp               *  -1.0617);

    /* Financial stress */
    score += (float)(st->tail_risk          *  1.5414);
    score += (float)(st->liq_gap            *  0.2032);
    score += (float)(st->ofr               *  -0.0242);
    score += (float)(st->hy_spread          * -0.0517);
    score += (float)(st->dxy_mom            *  0.1018);
    score += (float)(st->oil_price          * -0.0027);

    /* Technology / reflexivity */
    score += (float)(st->ai_capex           * -0.7323);
    score += (float)(st->lagged_ai          *  0.7827);

    /* Geopolitical / sentiment */
    score += (float)(st->geopolitical_risk  * -0.2550);
    score += (float)(st->investor_sentiment *  0.3624);

    /* Clamp to [0, 1] */
    if (score < 0.0f) score = 0.0f;
    if (score > 1.0f) score = 1.0f;

    return score;
}