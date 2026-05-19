#ifndef STATE_H
#define STATE_H

typedef struct {
    char   name[32];
    double int_rev;
    double debt_gdp;
    double usd_reserve_share;
    double cbo_deficit;
    double xdate;
    double sahm;
    double tail_risk;
    double liq_gap;
    double ofr;
    double hy_spread;
    double dxy_mom;
    double oil_price;
    double ai_capex;
    double geopolitical_risk;
    double investor_sentiment;
} Scenario;

typedef struct {
    double int_rev;
    double debt_gdp;
    double usd_reserve_share;
    double cbo_deficit;
    double xdate;
    double sahm;
    double tail_risk;
    double liq_gap;
    double ofr;
    double hy_spread;
    double dxy_mom;
    double oil_price;
    double ai_capex;
    double geopolitical_risk;
    double investor_sentiment;
    double lagged_ai;

    /* Required for calibration */
    double infl;
    double unemp;
    double gdp;
} State;

#endif
