#ifndef ENGINE_H
#define ENGINE_H

#include "state.h"

/*
 * Core QUANTXT evaluation function.
 * Takes a fully-populated State and returns a float score.
 */
float run_engine(const State *st);

#endif
