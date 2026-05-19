#include <graph.h>
#include <conio.h>
#include <stdio.h>
#include "state.h"
#include "system.h"
#include "colors.h"
#include "dash.h"

/* Dashboard width and centering offset */
#define DASH_WIDTH 60
#define DASH_OFFSET ((80 - DASH_WIDTH) / 2)

/* ---------------------------------------------------------
 * Risk bar renderer
 * --------------------------------------------------------- */
static void draw_risk_bar(double value, int row, int col)
{
    int length;
    int i;

    length = (int)(value * 10.0);
    if (length > 10) length = 10;
    if (length < 0)  length = 0;

    if (value < 0.5)
        _settextcolor(COL_GOOD);
    else if (value < 0.8)
        _settextcolor(COL_WARNING);
    else
        _settextcolor(COL_CRITICAL);

    _settextposition(row, DASH_OFFSET + col);

    for (i = 0; i < 10; i++) {
        _outtext(i < length ? "#" : "-");
    }

    _settextcolor(COL_NORMAL);
}

/* ---------------------------------------------------------
 * draw_dashboard — centered XT‑style dashboard
 * --------------------------------------------------------- */
void draw_dashboard(const SystemOut *o, const State *s)
{
    char buf[32];

    _clearscreen(_GCLEARSCREEN);

    /* HEADER */
    _settextcolor(COL_HEADER);
    _settextposition(1, DASH_OFFSET + 5);
    _outtext("FISCAL DOMINANCE DASHBOARD");
    _settextcolor(COL_NORMAL);

    /* INPUTS */
    _settextposition(3, DASH_OFFSET + 0);
    _outtext("Inputs:");

    _settextposition(5, DASH_OFFSET + 2);
    _outtext("Debt/GDP:            ");
    sprintf(buf, "%g", s->debt_gdp);
    _outtext(buf);

    _settextposition(6, DASH_OFFSET + 2);
    _outtext("Interest/Revenue:    ");
    sprintf(buf, "%g", s->int_rev);
    _outtext(buf);

    _settextposition(7, DASH_OFFSET + 2);
    _outtext("USD Reserve Share:   ");
    sprintf(buf, "%g", s->usd_reserve_share);
    _outtext(buf);

    _settextposition(8, DASH_OFFSET + 2);
    _outtext("CBO Deficit:         ");
    sprintf(buf, "%g", s->cbo_deficit);
    _outtext(buf);

    _settextposition(9, DASH_OFFSET + 2);
    _outtext("X-Date (days):       ");
    sprintf(buf, "%g", s->xdate);
    _outtext(buf);

    _settextposition(10, DASH_OFFSET + 2);
    _outtext("Sahm Rule:           ");
    sprintf(buf, "%g", s->sahm);
    _outtext(buf);

    /* MODULE / RISK VIEWS */
    _settextposition(12, DASH_OFFSET + 0);
    _outtext("Risk Views:");

    _settextposition(14, DASH_OFFSET + 2);
    _outtext("Tail Risk:        ");
    draw_risk_bar(s->tail_risk, 14, 22);
    sprintf(buf, "(%g)", s->tail_risk);
    _settextposition(14, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(15, DASH_OFFSET + 2);
    _outtext("Liquidity Gap:    ");
    draw_risk_bar(s->liq_gap, 15, 22);
    sprintf(buf, "(%g)", s->liq_gap);
    _settextposition(15, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(16, DASH_OFFSET + 2);
    _outtext("Debt Pressure:    ");
    draw_risk_bar(o->debt_pressure, 16, 22);
    sprintf(buf, "(%g)", o->debt_pressure);
    _settextposition(16, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(17, DASH_OFFSET + 2);
    _outtext("OFR Index:        ");
    draw_risk_bar(s->ofr, 17, 22);
    sprintf(buf, "(%g)", s->ofr);
    _settextposition(17, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(18, DASH_OFFSET + 2);
    _outtext("HY Spread:        ");
    draw_risk_bar(s->hy_spread, 18, 22);
    sprintf(buf, "(%g)", s->hy_spread);
    _settextposition(18, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(19, DASH_OFFSET + 2);
    _outtext("DXY Momentum:     ");
    draw_risk_bar(s->dxy_mom, 19, 22);
    sprintf(buf, "(%g)", s->dxy_mom);
    _settextposition(19, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(20, DASH_OFFSET + 2);
    _outtext("Oil Price:        ");
    draw_risk_bar(s->oil_price, 20, 22);
    sprintf(buf, "(%g)", s->oil_price);
    _settextposition(20, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(21, DASH_OFFSET + 2);
    _outtext("AI Capex:         ");
    draw_risk_bar(s->ai_capex, 21, 22);
    sprintf(buf, "(%g)", s->ai_capex);
    _settextposition(21, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(22, DASH_OFFSET + 2);
    _outtext("Geo Risk:         ");
    draw_risk_bar(s->geopolitical_risk, 22, 22);
    sprintf(buf, "(%g)", s->geopolitical_risk);
    _settextposition(22, DASH_OFFSET + 35);
    _outtext(buf);

    _settextposition(23, DASH_OFFSET + 2);
    _outtext("Sentiment:        ");
    draw_risk_bar(s->investor_sentiment, 23, 22);
    sprintf(buf, "(%g)", s->investor_sentiment);
    _settextposition(23, DASH_OFFSET + 35);
    _outtext(buf);

    /* SYSTEM OUTPUT */
    _settextposition(25, DASH_OFFSET + 0);
    _outtext("System Output:");

    _settextposition(27, DASH_OFFSET + 2);
    _outtext("Raw Stress:       ");
    sprintf(buf, "%g", o->raw_stress);
    _outtext(buf);

    _settextposition(28, DASH_OFFSET + 2);
    _outtext("AI Damping:       ");
    sprintf(buf, "%g", o->ai_damping);
    _outtext(buf);

    _settextposition(29, DASH_OFFSET + 2);
    _outtext("Composite Stress: ");
    sprintf(buf, "%g", o->M8);
    _outtext(buf);

    _settextposition(30, DASH_OFFSET + 2);
    _outtext("Regime:           ");
    _outtext((char __far *)o->regime);

    _settextposition(31, DASH_OFFSET + 2);
    _outtext("PONR Probability: ");
    sprintf(buf, "%g", o->ponr_probability);
    _outtext(buf);

    /* FOOTER */
    _settextcolor(COL_FOOTER);
    _settextposition(24, DASH_OFFSET + 0);
    _outtext((char __far *)"ESC to return");
    _settextcolor(COL_NORMAL);

    while (getch() != 27) {
        /* wait for ESC */
    }
}
