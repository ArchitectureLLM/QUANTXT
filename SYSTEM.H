#ifndef SYSTEM_H
#define SYSTEM_H

#include "state.h"

/* ---------------------------------------------------------
 * Params — model configuration
 * --------------------------------------------------------- */
typedef struct {
    double ai_coef;

    double m1_w;
    double m2_w;
    double m3_w;
    double m4_w;
    double m5_w;
    double m6_w;
    double m7_w;
    double m10_w;
    double m13_w;

    double hysteresis;
    double terminal_persistence;
    double base_rate;
    double base_growth;
    double raw_c;
} Params;

/* ---------------------------------------------------------
 * SystemOut — output vector of SYSTEM_engine()
 * --------------------------------------------------------- */
typedef struct {
    /* Module scores */
    double M1;
    double M2;
    double M3;
    double M4;
    double M5;
    double M6;
    double M7;
    double M8;
    double M9;
    double M10;
    double M13;

    /* Composite metrics */
    double raw_stress;
    double ai_damping;

    /* Regime classification */
    char   regime[32];

    /* Derived metrics */
    double ponr_probability;
    double stress_dispersion;
    double debt_pressure;
} SystemOut;

/* ---------------------------------------------------------
 * Function prototypes
 * --------------------------------------------------------- */
void init_default_params(Params *p);

void SYSTEM_engine(
    const State *s,
    const Params *params,
    double prev_M8,
    int has_prev,
    int reflexive_mode,
    int periods_above,
    SystemOut *out
);

#endif
