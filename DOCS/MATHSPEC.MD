# QuantXT Mathematics Specification
A formal description of the QuantXT macro‑economic modeling engine.

---

## 1. Overview

QuantXT implements a deterministic, multi‑factor, nonlinear macro‑economic model:

- **Input:** scenario vector \( x \)
- **Output:** system stress score \( S \)
- **Implementation:** C89, 8087 FP, real‑mode DOS

Pipeline:



\[
x \rightarrow \text{normalize}(x) \rightarrow F_i \rightarrow S
\]



---

## 2. Notation

- Scenario vector:


\[
x = (x_1, x_2, \dots, x_n)
\]



- Normalized inputs:


\[
\tilde{x}_j \in [0,1]
\]



- Factor outputs:


\[
F_i = M_i(\tilde{x})
\]



- System output:


\[
S = \text{SYSTEM}(F_1, F_2, \dots, F_k)
\]



---

## 3. Input Layer — Scenario → State

Scenario files populate the internal `STATE` struct:

- Macro: inflation, growth, unemployment  
- Policy: rate stance, QE/QT  
- Market: spreads, volatility  
- Liquidity: funding stress  

Mathematically:



\[
x_j \leftarrow \text{scenario\_field}_j
\]



---

## 4. Normalization Layer — Utility Transforms

### 4.1 Clipping (`clip01_`)


\[
\tilde{x}_j = \min(1, \max(0, x_j))
\]



### 4.2 Log Compression (`LOGF_`)


\[
y = \log(1 + a|x|)\,\text{sign}(x)
\]



### 4.3 Smooth Step (`SMOOTH_STEP_`)


\[
s(t) =
\begin{cases}
0 & t \le a \\
3z^2 - 2z^3 & a < t < b,\; z=\frac{t-a}{b-a} \\
1 & t \ge b
\end{cases}
\]



### 4.4 Piecewise Linear (`PWL_`)
Linear interpolation between breakpoints \((x_k, y_k)\).

### 4.5 Spline‑like (`SPL_`)
Smooth interpolation across multiple knots.

---

## 5. Factor Layer — Modules \( M_1 \dots M_{13} \)

Each module computes a single economic factor:



\[
F_i = M_i(\tilde{x})
\]



General form:



\[
F_i = \sum_j w_{ij} T_{ij}(\tilde{x}_j)
      + \sum_{j,k} v_{ijk} T_{ijk}(\tilde{x}_j, \tilde{x}_k)
\]



Where \(T\) are transforms from Section 4.

Examples:

### \(M_1\): Inflation Pressure


\[
F_1 = w_{1,\pi}\,\text{SMOOTH\_STEP}(\tilde{\pi}) + \dots
\]



### \(M_2\): Credit Stress


\[
F_2 = w_{2,\text{cs}}\,\text{PWL}(\tilde{\text{spread}}) + \dots
\]



### \(M_3\): Liquidity Conditions  
### \(M_4\): Policy Stance  
### \(M_5\): Growth Expectations  
### \(M_6\): External Shocks  
### \(M_7\): Volatility  
### \(M_9\): Sentiment  
### \(M_{10}\): Structural Risk  
### \(M_{13}\): Higher‑Order Nonlinear Composite  

---

## 6. System Layer — Aggregation & Nonlinearity

### 6.1 Linear Mix


\[
L = \sum_i \alpha_i F_i
\]



### 6.2 Nonlinear Shaping


\[
L' = \text{SMOOTH\_STEP}(L; a, b)
\]



Optional convexity:



\[
L'' = (L')^\gamma
\]



### 6.3 Clamping


\[
S = \text{clip01}(L'')
\]



Final output \(S \in [0,1]\).

---

## 7. Calibration

Given calibration scenarios \(x^{(k)}\) with target scores \(S^{*(k)}\):

Model output:


\[
S^{(k)} = \text{SYSTEM}(M_i(\tilde{x}^{(k)}))
\]



Error:


\[
e^{(k)} = S^{(k)} - S^{*(k)}
\]



Aggregate error:


\[
E_{\text{total}} = \sum_k |e^{(k)}|
\]




\[
E_{\text{mean}} = \frac{1}{K}\sum_k |e^{(k)}|
\]



---

## 8. Output Mapping

- Dashboard displays:
  - stress score \(S\)
  - factor contributions \(F_i\)
  - regime classification

- QXGraph plots:
  - \(S(t)\)
  - factor overlays
  - scenario comparisons

---

# End of Document
