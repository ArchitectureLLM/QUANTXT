# QUANTXT
<img width="701" height="502" alt="Screenshot" src="DOCS/Screenshot.png" />

>Screenshot: QUANTXT intro screen running on an IBM 5160 (8087 FPU), displayed on an IBM 5153 Color Display.

**Sovereign Risk Modeling Engine**  
*C89 Standard· Watcom Open C V1.9 · Real-mode DOS · Intel 8087 FPU*

---
## **Releases**

[Latest Full Release can be found here](https://github.com/ArchitectureLLM/QUANTXT/releases)

---

> Built to answer a real question: given current US sovereign conditions, what does the data say the stress probability is?
> The answer runs on a 1981 coprocessor. Calibrated against 60 scenarios — MSE 0.0038, RMSE 0.062 on a [0,1] scale.
 

---

## **QUANTXT — TL;DR**

**QUANTXT** is a fully self‑contained **macro‑risk modeling engine** built in pure ANSI C89 for real DOS hardware.  
It loads economic scenarios, runs a deterministic **13‑factor stress model**, applies nonlinear system transforms, and renders results in a clean XT‑style dashboard UI.

- **Two‑pass stress engine** [Explain QUANTXT stress modules](https://github.com/ArchitectureLLM/QUANTXT/blob/main/README.md#modulesc--modulesh) with damping, smoothing, and regime classification  
- **Deterministic outputs** — no randomness, no external dependencies  
- **Runs on DOS, DOSBox, and retro toolchains** ([build instructions](https://github.com/ArchitectureLLM/QUANTXT#manual-build))  
- **BIOS‑text dashboard** with centered layout and color‑coded risk bars  
- **Lightweight, modular architecture** ([state.h](ca://s?q=Explain_QUANTXT_state_h)) as the shared root, no cycles  
- **Educational reference implementation** showing how modern risk concepts fit into 1980s constraints

QUANTXT is designed to be **transparent, inspectable, and easy to reason about** — a modern macro‑risk engine expressed through the simplicity of DOS‑era software engineering.

---

## The Full Explanation. 

QUANTXT is a deterministic, nonlinear, multi-factor macro-financial simulation engine implemented in C89 and executed on IBM XT-class hardware. It computes a sovereign risk index by aggregating 13 economic, political, and market-based factor modules through a weighted nonlinear pipeline with reflexive feedback.

It is currently an alpha demo. It is not a tutorial project. The goal is to build a deterministic analytical instrument — built to model US sovereign risk dynamics using real FRED macroeconomic data and market inputs, cross-referenced against contemporary macro frameworks including Paul Tudor Jones's bond market stress thesis and Garrett Jones's structural fiscal narrative. It is an attempt to build deterministic tooling for evaluating macroeconomic narratives under constrained and reproducible execution.

The engine originated as a YAML schema, was prototyped in Python, and was then ported into C89/8087 with successive tightening of constraints and calibration at each stage. Rather than relaxing assumptions during implementation, each layer forced stricter determinism and more explicit modeling. The most constrained environment ultimately produced the most stable behavior.

---

# QUANTXT Code Map

## Architecture Overview

```
MAIN
├── INTRO          (splash screen, no deps)
├── BROWSER        (scenario picker UI)
│   └── SCENARIO   (scenario registry + loader + manual input)
├── FILEBRO        (file picker UI, no module deps)
├── SCENARIO       (scenario registry, scenario_count, loader, manual input)
├── MODULES        (M1–M13 stress functions + compute_system_out)
│   └── state.h    (shared types: State, Scenario, Params, SystemOut)
├── SYSTEM         (SYSTEM_engine, init_default_params)
│   ├── MODULES    (pure stress modules, no cycle in v1.11)
│   ├── UTIL       (clip01, LOGF, SMOOTH_STEP, PWL, SPL)
│   ├── HISTORY    (run history ring buffer)
│   └── ENGINE     (OLS scoring engine)
├── DASH           (dashboard UI — BIOS text renderer)
├── QXCALIB        (calibration runner)
│   └── ENGINE
└── ENGINE         (run_engine — pure float scorer)

```

---

## Module Reference

### MAIN.C
**Entry point.** Menu loop: browse scenarios, manual input, load .TXT file, run calibration.  
Calls: `run_intro`, `browse_scenarios`, `manual_input`, `browse_txt_files`,
`load_scenario_file_multi`, `fill_state_from_scenario`, `compute_system_out`,
`draw_dashboard`, `qxcalib_run`

---

### INTRO.C / INTRO.H
**CGA splash screen.**  
640×200 mono vector logo, grid animation, BIOS tick timing.  
No dependencies on other QUANTXT modules.  
Exports: `run_intro(void)`

---

### BROWSER.C / BROWSER.H
**Scenario browser UI.**  
Scrollable list of loaded scenarios; returns selected index or -1 (ESC).  
Depends on: `state.h`, `system.h`, `scenario.h`, `colors.h`  
Exports: `browse_scenarios(void)`

---

### FILEBRO.C / FILEBRO.H
**DOS 8.3 file picker.**  
Scans `*.TXT` in working directory; scrollable picker returns filename.  
Depends on: `colors.h` only  
Globals: `file_list[64][13]`, `file_count`  
Exports: `browse_txt_files(char *out_filename)`, `load_txt_file_list(void)`

---

### SCENARIO.C / SCENARIO.H
**Scenario registry + loader.**  
Holds built-in scenarios (`gfc_2008`, `debtceiling_2011`, `today_2026`).  
Loads external `.TXT` packs; provides manual input UI.  
Depends on: `state.h`, `colors.h`  
Globals: `scenarios[256]`, `scenario_count`  
Exports: `fill_state_from_scenario`, `manual_input`, `load_scenario_file_multi`

---

### ENGINE.C / ENGINE.H
**OLS scoring engine.**  
Pure function: takes `State`, returns `float` score in [0, 1].  
19 OLS-calibrated weights (RMSE 0.020, 30-point precedent set).  
Depends on: `state.h` only  
Exports: `run_engine(const State *st)`

---

### **MODULES.C / MODULES.H**
**Stress sub‑modules M1–M13 + `compute_system_out`.**  
Each M‑function is pure, deterministic, and returns a normalized **[0–1]** stress score.  
Evaluation order is dependency‑aware: **M3 → M1**, **M5 → M2**, **M9 → M6**.  
Now depends only on **`state.h`** (circular dependency with SYSTEM removed in v1.11).  
Uses the canonical `clip01()` from **`util.h`** (duplicate local version removed).  
Exports: **`M1`–`M13`**, `compute_system_out(const State*, SystemOut*)`.

| Function | Inputs | Notes |
|---|---|---|
| M1 | int_rev, M3_score | Interest burden |
| M2 | tail_risk, liq_gap, M5_score | Liquidity/tail risk |
| M3 | xdate, cbo_deficit | X-date + fiscal |
| M4 | usd_reserve_share | USD reserve decline |
| M5 | ofr, hy_spread | Credit stress |
| M6 | sahm, M9_score, lagged_ai | Cycle + AI reflexivity |
| M7 | dxy_mom, oil_price | Dollar + oil shock |
| M9 | ai_capex | AI capex stabilizer |
| M10 | geopolitical_risk | Geo risk |
| M13 | investor_sentiment | Sentiment (contrarian) |

---

### **SYSTEM.C / SYSTEM.H**
**Full nonlinear macro‑risk engine.**  
Executes M‑modules in two passes, applies AI‑capex damping, EMA smoothing, regime classification, and PONR probability estimation.  
Now depends on: **`modules.h`, `util.h`, `history.h`, `engine.h`** — with the previous circular dependency on MODULES removed in v1.11.  
Exports: **`init_default_params(Params*)`**, `SYSTEM_engine(const State*, const Params*, SystemOut*)`.


**Two-pass evaluation:**
```
Pass 1 (independent): M3, M4, M5, M7, M9, M10, M13
Pass 2 (dependent):   M1(←M3), M2(←M5), M6(←M9)
```

---

### **DASH.C / DASH.H**
**XT‑style dashboard UI.**  
Centered 60‑column layout with color‑coded risk bars and a clean BIOS‑text renderer.  
Page‑flip recursion removed in v1.11 — `draw_dashboard()` is now non‑recursive and stack‑safe.  
Depends on: **`state.h`**, **`system.h`**, **`colors.h`**  
Exports: **`draw_dashboard(const SystemOut*, const State*)`**

---

### QXCALIB.C / QXCALIB.H
**Calibration runner.**  
Parses `CALIB.TXT` key=value precedent blocks, runs `run_engine` against each,
computes SSE/MSE, writes `CALIB_RESULT.TXT`.  
Depends on: `engine.h`, `state.h`  
Exports: `qxcalib_run(precedent_file, result_file, QXCalibResult*)`

---

### UTIL.C / UTIL.H
**Math utilities.**  
No QUANTXT dependencies — safe to include anywhere.  
Exports:

| Function | Signature | Purpose |
|---|---|---|
| `clip01` | `(double x)` | Clamp to [0, 1] |
| `LOGF` | `(x, mid, k)` | Logistic sigmoid |
| `SMOOTH_STEP` | `(x, th[], n, k)` | Hermite multi-threshold step |
| `PWL` | `(x, bp[], slopes[], n)` | Piecewise linear |
| `SPL` | `(x, knots[], vals[], n, irreversible, *state)` | Linear spline + optional ratchet |

---

### HISTORY.C / HISTORY.H
**Run history ring buffer.**  
Stores up to 256 entries of (M8, M9, M10) per run.  
No QUANTXT dependencies.  
Exports: `history_reset(History*)`, `history_append(History*, m8, m9, m10)`

---

### STATE.H
**Shared data types — no logic, no dependencies.**  
Defines `Scenario` (16 fields + name) and `State` (19 fields including `lagged_ai`, `infl`, `unemp`, `gdp`).

### COLORS.H
**CGA/EGA/VGA color constants.**  
Standard palette (0–15) + semantic UI aliases (`COL_NORMAL`, `COL_HEADER`, etc.).  
No dependencies.

---

## Known Issues (Resolved in v1.11)

| # | File | Issue | Status |
|---|------|-------|--------|
| 1 | `SYSTEM.H` ↔ `MODULES.H` | Circular dependency | **Fixed** — `Params` & `SystemOut` moved to `state.h` |
| 2 | `MODULES.C` | Duplicate `clip01()` | **Fixed** — Now uses canonical version from `util.h` |
| 3 | `DASH.C` | Recursive `draw_dashboard()` | **Fixed** — Converted to non-recursive page loop |

All major architectural issues resolved in v1.11. The codebase is now clean and stable.


---

## Data Flow (happy path)

```
startup  
  └─ run_intro()  
  └─ main menu loop  
       ├─ browse_scenarios() / manual_input() / load_scenario_file_multi()  
       │       └─ fills **State**  
       ├─ compute_system_out(State → SystemOut)   [MODULES]  
       │       └─ M3, M4, M5, M7, M9, M10, M13  (pass 1)  
       │       └─ M1, M2, M6                    (pass 2)  
       │       └─ weighted sum → raw_stress  
       │       └─ AI damping → M8  
       │       └─ regime classification  
       └─ draw_dashboard(SystemOut, State)       [DASH]

```

---

## **File Index (v1.11)**

| File            | Role                               | Deps (user headers) |
|-----------------|-------------------------------------|----------------------|
| `state.h`       | Core types (`State`, `Scenario`, `Params`, `SystemOut`) | — |
| `colors.h`      | Color constants                     | — |
| `util.h/c`      | Math helpers (clip01, LOGF, SMOOTH_STEP, PWL, SPL) | — |
| `history.h/c`   | Run‑history ring buffer             | — |
| `engine.h/c`    | OLS scoring engine (`run_engine`)   | `state.h` |
| `intro.h/c`     | Splash screen                       | — |
| `filebro.h/c`   | File picker UI                      | `colors.h` |
| `scenario.h/c`  | Scenario registry, loader, manual input | `state.h`, `colors.h` |
| `modules.h/c`   | M1–M13 stress modules + aggregator  | `state.h` |
| `system.h/c`    | Full macro‑risk engine              | `state.h`, `modules.h`, `util.h`, `history.h`, `engine.h` |
| `dash.h/c`      | XT‑style dashboard UI               | `state.h`, `system.h`, `colors.h` |
| `browser.h/c`   | Scenario browser UI                 | `state.h`, `scenario.h`, `colors.h` |
| `qxcalib.h/c`   | Calibration runner                  | `state.h`, `engine.h` |
| `main.c`        | Entry point                         | all of the above |

---

## Mathematical Model

**Pipeline:** `x → normalize(x) → Fᵢ → S`

### Normalization Layer (`UTIL.C`)
| Transform | Function |
|-----------|----------|
| `clip01` | Bounds inputs to [0,1] |
| `LOGF` | Log compression `y = log(1+a\|x\|)·sign(x)` |
| `SMOOTH_STEP` | Cubic S-curve `s(t) = 3z² - 2z³` |
| `PWL` | Piecewise linear between calibrated breakpoints |
| `SPL` | Spline-like smoothing across multiple knots |

### Factor Layer (`MODULES.C`) — M1–M13
Each module is a pure function. Includes cross-factor interaction terms:

```
Fᵢ = Σⱼ wᵢⱼ Tᵢⱼ(x̃ⱼ)  +  Σⱼ,ₖ vᵢⱼₖ Tᵢⱼₖ(x̃ⱼ, x̃ₖ)
```

| Module | Factor |
|--------|--------|
| M1 | Inflation pressure |
| M2 | Credit stress |
| M3 | Liquidity conditions |
| M4 | Policy stance |
| M5 | Growth expectations |
| M6 | External shocks |
| M7 | Volatility |
| M9 | Sentiment |
| M10 | Structural risk |
| M13 | Higher-order nonlinear composite |

### System Layer (`SYSTEM.C`)
```
L   = Σᵢ αᵢ Fᵢ                    — weighted linear mix
L'  = SMOOTH_STEP(L; a, b)         — nonlinear shaping
L'' = (L')ᵞ                        — optional convexity
S   = clip01(L'')                   — S ∈ [0,1]
```

**Reflexive feedback:** M8 output at period *t* feeds forward into period *t+1*, capturing the self-reinforcing dynamics of sovereign stress episodes. Damping functions stabilize the loop.

### Outputs
| Output | Description |
|--------|-------------|
| `S` | Stress score ∈ [0,1] |
| Regime | Low / Medium / High Risk |
| PONR | Point-of-no-return crisis threshold probability |

---

## Calibration

Calibrated against 60 scenarios spanning the full US economic cycle — from early expansion through severe crisis and back through recovery — using FRED macroeconomic series and market data.

| Metric | Value |
|--------|-------|
| Scenarios (initial) | 30 |
| Mean error (initial) | 0.135473 |
| Scenarios (current) | 60 |
| Total error (current) | 0.228393 |
| **Mean error (current)** | **0.003807** |
| Error reduction | ~35× improvement |
| Data sources | FRED + market data |

At mean error **0.0038** on a 0–1 scale, the model is hitting targets with sub-percent accuracy across the full economic cycle.

---

## SCENARIO

Scenarios are in the following format. The goal will be to soon unify the Scenario and Calibration data formats. 
```
#name int_rev debt_gdp usd_reserve_share cbo_deficit xdate sahm tail_risk liq_gap ofr hy_spread dxy_mom oil_price ai_capex geopolitical_risk investor_sentiment

managed_norm_1 22 70 65 3 90 0.2 1.2 1.0 0.95 600 5 80 1.0 0.5 0.4
managed_norm_2 23 72 66 3.2 88 0.18 1.1 1.05 0.96 580 4 82 1.1 0.45 0.42
managed_norm_3 21 68 64 2.8 92 0.22 1.3 0.98 0.94 620 6 78 0.9 0.55 0.38
```
---

## Input Vector — 19-Field Sovereign Risk Schema

| Field | Description | Range |
|-------|-------------|-------|
| `int_rev` | Interest as share of revenue | 0.0 – 1.0 |
| `debt_gdp` | Debt as share of GDP | 0.0 – 2.0 |
| `usd_reserve_share` | USD share of global reserves | % |
| `cbo_deficit` | CBO projected deficit | % GDP |
| `xdate` | Days to debt ceiling breach | 0 – 360 |
| `sahm` | Sahm rule recession indicator | 0.0 – 1.0 |
| `tail_risk` | Tail risk index | 0.0 – 1.0 |
| `liq_gap` | Liquidity gap index | 0.0 – 1.0 |
| `ofr` | OFR financial stress index | 0.0 – 1.0 |
| `hy_spread` | High yield spread index | 0.0 – 1.0 |
| `dxy_mom` | DXY momentum index | 0.0 – 1.0 |
| `oil_price` | Crude oil price | USD/barrel |
| `ai_capex` | AI capital expenditure index | 0.0 – 5.0 |
| `lagged_ai` | Prior period AI capex | 0.0 – 5.0 |
| `geopolitical_risk` | Geopolitical risk index | 0.0 – 1.0 |
| `investor_sentiment` | Investor sentiment index | 0.0 – 1.0 |
| `infl` | Inflation rate | 0.0 – 1.0 |
| `unemp` | Unemployment rate | 0.0 – 1.0 |
| `gdp` | GDP growth rate | -0.1 to 0.1 |

---

## Calibration File Format

```
# Scenario Name
int_rev=0.32
debt_gdp=0.95
usd_reserve_share=57.0
cbo_deficit=11.0
xdate=20
sahm=0.40
tail_risk=0.90
liq_gap=0.85
ofr=0.90
hy_spread=0.88
dxy_mom=0.70
oil_price=60.0
ai_capex=0.15
lagged_ai=0.20
geopolitical_risk=0.75
investor_sentiment=0.15
infl=0.02
unemp=0.12
gdp=-0.05
target=0.92
```

Scenarios are separated by blank lines. Comments begin with `#` or `;`.

---

## Build

```bash
# QUANTXT — Build Instructions

## Requirements

| Tool | Version | Notes |
|------|---------|-------|
| [Open Watcom C](https://github.com/open-watcom/open-watcom-v2/releases) | V1.9 | Primary toolchain |
| DOS or DOSBox | Any | For running the compiled binary |

> **DOSBox** is the recommended environment for modern systems (Windows, macOS, Linux).  
> Download: https://www.dosbox.com/

---

## Quick Build (Makefile)

If you have Open Watcom installed and the repo cloned, the simplest path is:

```bat
wmake -f QUANTXT.mk
```

This compiles all modules and links `QUANTXT.EXE` in one step.

---

## Manual Build

### Step 1 — Compile each module

Run `wcc` on each `.C` file. Replace `C:\WATCOM\h` with your Watcom include path if different.

```bat
wcc BROWSER.C  -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc DASH.C     -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc ENGINE.C   -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc FILEBRO.C  -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc HISTORY.C  -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc INTRO.C    -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc MAIN.C     -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc MODULES.C  -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc QXCALIB.C  -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc SCENARIO.C -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc SYSTEM.C   -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
wcc UTIL.C     -i="C:\WATCOM\h" -w4 -e25 -zq -od -d2 -fpi87 -bt=dos -fo=.obj -ml
```

### Step 2 — Link

```bat
wlink name QUANTXT sys dos op st=16384 op m op maxe=25 op q op symf FILE BROWSER.obj,DASH.obj,ENGINE.obj,FILEBRO.obj,HISTORY.obj,INTRO.obj,MAIN.obj,MODULES.obj,QXCALIB.obj,SCENARIO.obj,SYSTEM.obj,UTIL.obj
```

---

## Compiler Flag Reference

| Flag | Meaning |
|------|---------|
| `-ml` | Large memory model — **required** |
| `-fpi87` | Inline 8087 FPU instructions — **required** |
| `-bt=dos` | Target real-mode DOS |
| `-w4` | Warning level 4 |
| `-e25` | Stop after 25 errors |
| `-zq` | Quiet mode |
| `-od` | Disable optimization (debug-safe) |
| `-d2` | Full debug info |

## Linker Flag Reference

| Flag | Meaning |
|------|---------|
| `sys dos` | Real-mode DOS executable |
| `op st=16384` | Stack size 16 KB — **required** |
| `op m` | Generate map file |
| `op maxe=25` | Stop after 25 errors |
| `op q` | Quiet mode |
| `op symf` | Generate symbol file |

---

## Running on Modern Systems (DOSBox)

1. Install [DOSBox](https://www.dosbox.com/)
2. Launch DOSBox and mount your build directory:

```
mount c C:\path\to\QUANTXT
c:
QUANTXT.EXE
```

3. The splash screen will load, followed by the main menu.

> On a real IBM PC/XT the calibration routine will pause briefly while loading 60 scenarios — this is expected.

---

## Tested Environment

| Component | Spec |
|-----------|------|
| Compiler | Open Watcom C V1.9 |
| Target OS | DOS 6.22 |
| Hardware | IBM PC/XT or compatible |
| FPU | Intel 8087 (or DOSBox emulation) |
| RAM | 256 KB minimum |
| Display | CGA |


**Requirements:**
- Watcom C/C++ (`wcc` / `wlink`)
- Real-mode DOS (or DOSBox for modern systems)
- Memory model: large (`-ml`)

**System requirements:**
- IBM PC/XT or compatible (8087 for Math CoProcessor acceleration)
- 8087 recommended for full precision
- 256 KB RAM minimum
- CGA graphics
- DOS 3.3+

---

## Usage

| Key | Action |
|-----|--------|
| `1` | Browse scenario files |
| `2` | Manual Entry of Data |
| `3` | Load Scenario File |
| `4` | Run Calibration File* | 
| `ENTER` | Run engine |
| `ESC` | Quit |

* Screen will pause for a few seconds on an IBM PC as it loads 60 sets of calibration data in to the model
---

## Development History

| Phase | Technology | Purpose |
|-------|-----------|---------|
| I | YAML | Parameter schema; model contract definition; PTJ-Weimar benchmark |
| II | Python 3 | Mathematical validation; scenario runner; output format lock-in |
| III | C89 / 8087 | Production engine; deterministic execution; calibration hardening |

The flat-file TXT format served as the binding data contract across all three phases. AI Scalped scenarios feed directly into `QUANTXT.EXE` without transformation.

---

## Why C89 on XT Hardware

The constraint is the point.

An engineer who can build a calibrated, nonlinear, multi-factor sovereign risk engine within the constraints of a 1981 coprocessor understands the computation — not just the tools. No runtime. No garbage collector. No dynamic loader. No abstraction noise between the mathematics and the metal.

The 8087's 80-bit extended precision and real-mode determinism produced *tighter* calibration than Python floating point. The constraints proved that a big idea could be ported to one of the earliest iterations of home PC.

> *If it works on an IBM XT, it works anywhere.*

---

## File Structure

```
QUANTXT/
├── MAIN.C          — Orchestrator
├── ENGINE.C        — Computational hub
├── MODULES.C       — M1–M13 factor modules
├── SYSTEM.C        — Aggregation + regime classification
├── SCENARIO.C      — Scenario loader → STATE struct
├── FILEBRO.C       — File browser + I/O
├── DASH.C          — ASCII dashboard renderer
├── QXCALIB.C       — Calibration engine
├── HISTORY.C       — Scenario state tracking
├── UTIL.C          — Math transforms (clip01, LOGF, SMOOTH_STEP, PWL, SPL)
├── STATE.H         — Core struct definitions
├── ENGINE.H        — Engine interface
├── MODULES.H       — Module interfaces
├── SYSTEM.H        — System layer interface
├── QXCALIB.H       — Calibration interface
├── UTIL.H          — Utility function declarations
├── COLORS.H        — CGA color constants
├── QUANTXT.MK      — Watcom makefile
├── CALIB.TXT       — 60-scenario calibration dataset

```

---

## Status

- [x] Core engine — stable
- [x] 60-scenario calibration — mean error 0.0038
- [x] YAML → Python → C89/8087 evolution complete
- [x] CGA QXGraph visualization operational
- [x] Multi-scenario TXT loader
- [x] Header architecture hardened (debugging sprint complete)
- [x] Multi variable scenario simulations used for calibrating the dataset, and more to be performed over time.
- [ ] Developing Forecast modeling on sequential daily data
- [ ] Improving Regime transition validation on real observed path
- [ ] Current US environment live stress assessment
- [ ] QXGraph multi-parameter overlay + zoom

---

## Author

**ArchitectureLabs**
Systems engineer. 16 years industrial operations experience.  Building quantitative tools at the intersection of macro-financial analysis and constraint-driven systems engineering.

---

## Changelog
# QUANTXT v1.11 — Stability & Architecture Cleanup Release

Release Date: May 2026

Focus: Architecture cleanup, dependency fixes, dashboard stability, and UI alignment improvements.

Overview
Version 1.11 is a structural and stability-focused update that resolves long‑standing architectural issues, removes circular dependencies, stabilizes the dashboard renderer, and improves the visual layout of the XT‑style interface. This release does not change model logic — it strengthens the foundation the engine runs on.

Key Fixes & Improvements
1. Removed SYSTEM.H ↔ MODULES.H Circular Dependency
Centralized all shared types (State, Scenario, Params, SystemOut) into state.h.

modules.h and system.h now depend only on state.h, eliminating recursive include chains.

Build is now deterministic under Watcom and easier to maintain.

2. Removed Duplicate clip01() Implementation
Deleted the shadowed static version inside modules.c.

The engine now uses the canonical implementation from util.h.

Prevents inconsistent clipping behavior across modules.

3. Eliminated Recursive draw_dashboard() Call
Removed the page‑flip recursion that previously caused stack growth.

Dashboard rendering is now linear, predictable, and safe.

4. Replaced Direct VRAM Writes With BIOS Text Rendering
Removed fragile CGA memory writes that caused screen bleed and instability.

Switched to _outtext()‑based rendering for full compatibility across DOSBox, CGA, EGA, and VGA.

Dashboard is now flicker‑free and stable.

5. Centered Bargraph Layout
Introduced DASH_BAR_WIDTH and DASH_BAR_OFFSET for clean horizontal centering.

Bars now align visually with the dashboard header and numeric columns.

Produces a more balanced and readable XT‑style UI.

## QUANTXT v1.12 — Stability, Precision & Calibration Release

Release Date: May 21, 2026

Focus: Floating-point precision corrections, calibration engine upgrade,
file browser improvements, and UI stability fixes.

### Precision Fixes
- Corrected float → double throughout ENGINE.C, ENGINE.H, and
  QXCALIB.H for correct 8087 FPU precision. State fields were
  already double; ENGINE was the remaining inconsistency.
- Removed erroneous (float) casts in ENGINE.C weight accumulation.
- QXCalibResult total_error and mean_error promoted to double.

### Calibration Engine Upgrade
- Calibration option now opens a file browser to select any .TXT
  calibration dataset rather than being hardcoded to CALIB.TXT.
- In-progress screen with blinking status message shown during run
  so the engine does not appear frozen on real hardware.
- Full Phase 1 diagnostic report now written to CALIB_RESULT.TXT:
  aggregate metrics (SSE, MSE, RMSE), bias analysis, regime
  breakdown across 5 stress bands, and per-scenario detail table
  with flagged outliers.
- Post-run results displayed on screen with spacebar paging.

### Browser Fixes
- BROWSER.C: fixed viewport scroll bug where scenarios beyond row
  25 were not visible. Added top/sel coupled scrolling, PgUp/PgDn,
  scroll indicators, and scenario counter.
- FILEBRO.C: same viewport fix applied. Added scroll indicators,
  PgUp/PgDn, file counter, and empty directory message.

  # QUANTXT v1.12 → v1.14 Changelog

## Executive Summary

QUANTXT v1.14 consolidates the calibration pipeline, fixes critical parameter handling issues, and stabilizes scenario loading. The core engine is now production-ready for daily market observation workflows.

---

## Critical Fixes

### 1. **Parameter Initialization & MODL.TXT Corruption**
- **Issue**: Uninitialized CalibParams in qxcalib_run() led to garbage values in MODL.TXT
- **Fix**: Added safe default initialization (0.85, 0.72, 1.20, ...) before loading from disk
- **Impact**: Calibration now produces valid, stable model parameters on every run

### 2. **Build System Stability (C89/Watcom C 1.9)**
- Resolved duplicate typedef conflicts (CalibParamsTag, Scenario)
- Added include guards to prevent redeclaration
- Cleaned scenario.c to eliminate struct assignments and static initializers with non-constants
- **Result**: Clean compile with zero critical warnings on IBM PC XT target

### 3. **Engine Signature Correction**
- Fixed run_engine() calls in qxcalib_compute() to pass both State and CalibParams
- Aligned all engine invocations across main.c, qxcalib.c, and dashboard code
- **Impact**: Consistent parameter flow through calibration and execution paths

---

## New Features

### 1. **Closed Calibration Loop**
- Calibration engine now produces updated MODL.TXT with first-order parameter adjustment
- Adjustment rule: `new_param = old_param * (1 - mean_signed_error)`
- CALIB_AR.TXT (diagnostic output) remains separate from MODL.TXT (control input)
- **Workflow**: Load MODL.TXT → run scenarios → calibrate → update MODL.TXT → repeat

### 2. **Scenario File Format Consolidation**
- Old format (SCENARIO.TXT): 15 fields (space-delimited)
- New format (CALIB2N): 20 fields (key=value, blank-line separated)
- Dual-loader: automatically detects format and loads correctly
- **Compatibility**: Existing scenario files still work; new format supports richer metadata

### 3. **Daily Market Observation Support**
- Added USMAY10D.TXT / USMAY30D.TXT formats for consecutive-day market snapshots
- Derived missing fields: lagged_ai, infl, unemp, gdp
- Scenarios now loadable directly into engine for real-time market analysis

---

## Calibration Quality

**Test run (46 scenarios):**
- Total SSE: 0.3143
- RMSE: 0.0827
- Mean signed error: +0.0118 (slight over-prediction)
- Regime breakdown balanced across 5 risk regimes

**Updated MODL.TXT parameters** (from calibration):
```
m1_w=0.8399802924
m3_w=0.7115127183
m8_mid=1.1858545304
m8_k=0.8893908978
ai_coef=0.4941060543
hysteresis=0.2964636326
regime_penalty=0.7905696870
liquidity_floor=0.3952848435
tail_risk_scale=1.0870333196
```

---

## Technical Debt Addressed

1. ✓ Removed struct assignment overhead (C89 compatibility)
2. ✓ Eliminated static initializers with non-constant expressions
3. ✓ Fixed 8087 coprocessor precision handling in parameter updates
4. ✓ Reduced memory footprint for DOS XT compatibility
5. ✓ Standardized file I/O error handling

---

## Testing & Validation

- **Compile**: Clean build with Watcom C 1.9
- **Calibration**: 46-scenario precedent file, converged in single pass
- **Scenario Loading**: SCENARIO.TXT, CALIB2N.TXT, USMAY10D/30D formats verified
- **Dashboard**: Real-time computation and display stable on XT hardware (emulated)

---

## Breaking Changes

None. v1.14 is fully backward-compatible with v1.12 scenario files and workflows.

---

## Known Limitations

- Single-threaded calibration (DOS constraint)
- Fixed scenario buffer (MAX_SCENARIOS=100)
- Precision: 8087 double-precision (not extended)
- File I/O: DOS text-mode only

---

## Next Steps (v1.15+)

- Extend calibration to multi-regime parameter tuning
- Add persistence layer for parameter history
- Implement incremental scenario loading for large datasets
- Forecast module (conditional on stable parameters)

---

**Version**: QUANTXT v1.14 (IBM PC XT Edition)  
**Build**: Watcom C 1.9 / DOS 6.22  
**Date**: May 2026  
**Status**: Production Ready

### General
- Apache License 2.0 header added to source files.
- Version references updated to v1.12 throughout.

*QUANTXT v1.12 — May 21 2026*

QUANTXT is licensed under the Apache-2.0 License. See LICENSE for details.
