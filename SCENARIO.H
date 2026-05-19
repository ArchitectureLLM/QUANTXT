#ifndef SCENARIO_H
#define SCENARIO_H

#define MAX_SCENARIOS 256

#include "state.h"

/*
 * scenario.h — Scenario registry + loader interface
 *
 * Provides:
 *   - scenarios[]: global registry of built‑in + loaded scenarios
 *   - scenario_count: number of active scenarios
 *   - fill_state_from_scenario(): hydrates a State from a Scenario
 *   - manual_input(): interactive XT‑style manual entry
 *   - load_scenario_file_multi(): loads multiple [scenario] blocks
 *                                 from a .TXT file into scenarios[]
 *
 * The loader appends new scenarios to the global registry and
 * increments scenario_count. It does NOT clear existing scenarios.
 *
 * This header intentionally avoids UI dependencies.
 */

/* Forward declaration (Scenario is defined in state.h) */
typedef struct Scenario Scenario;

/* Global scenario registry */
extern Scenario scenarios[MAX_SCENARIOS];
extern int scenario_count;

/* Hydrate a State from a Scenario */
void fill_state_from_scenario(const Scenario *sc, State *s, double lagged_ai);

/* Manual entry UI */
void manual_input(State *s);

/* Multi‑scenario TXT loader */
int load_scenario_file_multi(const char *filename);

#endif
