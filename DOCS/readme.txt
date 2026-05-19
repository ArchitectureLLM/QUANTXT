# QUANTXT v1.1 — README  
Macro‑Risk Modeling Engine  
IBM PC/XT Compatible — CGA Mode 6  
Build Date: May 18, 2026

------------------------------------------------------------

## 1. PROJECT OVERVIEW
QUANTXT is a retro‑systems macro‑risk modeling engine designed for DOS‑era hardware.  
It runs on 8088/8086 machines, uses CGA Mode 6 vector graphics, and compiles under  
Watcom C using real‑mode memory models.

The system loads macroeconomic scenarios, applies nonlinear transformations, and  
produces regime‑probability outputs. It includes a calibration module, scenario  
browser, manual entry mode, and a CGA boot‑style intro screen.

------------------------------------------------------------

## 2. CURRENT Files

### INTRO.C  
- CGA 640×200 vector logo renderer  
- Scanline fade‑in effect  
- Centered BIOS text system  
- Parameterized layout (LOGO_LEFT, GRID_LEFT, etc.)  
- Fully symmetric, resolution‑independent design

### MAIN.C  
- QUANTXT main menu  
- Centered menu aligned to QUANTXT logo left boundary  
- Options for scenarios, calibration, manual entry, and help

### BROWSER.C  
- Lists `.TXT` scenario files  
- Displays preview of scenario contents  
- Loads selected scenario into engine

### ENGINE.C  
- Core nonlinear transformation engine  
- Computes regime probability  
- Applies curvature, hysteresis, and stability metrics

### FILEBRO.C  
- File I/O utilities  
- Scenario loader  
- Calibration loader (key=value format)

### QXCALIB.C  
- Loads CALIB.TXT  
- Computes total error and mean error  
- Supports multi‑scenario calibration sets (5–100 entries)

### DASH.C / HISTORY.C  
- Output formatting  
- Historical scenario tracking

### UTIL.C  
- Math helpers  
- String parsing  
- XT‑safe timing utilities

------------------------------------------------------------

## 3. INTRO SCREEN STATUS
The QUANTXT intro screen is now fully implemented:

- CGA vector logo (QUANTXT)  
- Perfectly centered using LOGO_LEFT  
- 80s‑style grid aligned to logo span  
- BIOS text centered horizontally  
- Scanline fade‑in  
- XT‑authentic timing using BIOS ticks  

This serves as the visual identity for the engine.

------------------------------------------------------------

## 4. SCENARIO FILE FORMAT (MARKET DATA)
Scenario files such as `USMAY10D.TXT` use a **15‑field row format**:

```
#name int_rev debt_gdp usd_reserve_share cbo_deficit xdate sahm tail_risk liq_gap ofr hy_spread dxy_mom oil_price ai_capex geopolitical_risk investor_sentiment

day_01 22.10 70.10 65.0 3.05 90 0.20 1.25 1.02 0.96 605 5.1 81 1.02 0.52 0.39
```

Each row represents a daily macro‑market snapshot.  
The engine extracts the fields it needs for model computation.

------------------------------------------------------------

## 5. CALIBRATION FILE FORMAT (CALIB.TXT)
Calibration files use a **key=value block format**:

```
# Scenario Name
int_rev=0.32
debt_gdp=0.68
sahm=0.45
infl=0.03
unemp=0.09
gdp=-0.02
target=0.92
```

This format is used only for model error computation.

Recent calibration results (30‑scenario set):
- Total error: 4.064188  
- Mean error: 0.135473  

------------------------------------------------------------

## 6. HELP SYSTEM
QUANTXT includes a built‑in text viewer that loads `MANUAL.TXT`.  
The viewer supports paging, 80×25 display, and XT‑safe file reading.

------------------------------------------------------------

## 7. BUILD SYSTEM
QUANTXT builds using Watcom C:

- `wcc` for compilation  
- `wlink` for DOS real‑mode linking  
- Memory model: **large** (`-ml`)  
- CGA graphics via `<graph.h>`  
- BIOS timing via `<bios.h>`

Example build line:

```
wmake -f QUANTXT.mk -h -e QUANTXT.exe
```

------------------------------------------------------------

## 8. SYSTEM REQUIREMENTS
- IBM PC/XT or compatible  
- 256 KB RAM  
- CGA graphics  
- DOS 3.3+   

------------------------------------------------------------

## 9. CURRENT STATUS SUMMARY
- Intro screen: **Complete**  
- Menu system: **Centered + aligned**  
- Scenario loader: **Functional**  
- Calibration engine: **Functional**  
- 30‑scenario calibration set: **Integrated**  
- Help system: **Operational**  
- Graphics + timing: **XT‑authentic**  

QUANTXT v1.1 is now a fully working retro macro‑modeling environment.

------------------------------------------------------------

## END OF README  
May 18, 2026
