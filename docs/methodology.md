# Cryogenic H2 Dispersion Model — Methodology

**Project:** NavHys — CFD validation of low-temperature and cryogenic H2 release  
**Platform:** OpenFOAM v2406, WSL2  
**Validation dataset:** Hecht & Panda (2019), Sandia National Laboratories

---

## 1. Physical Problem

Cryogenic hydrogen (50–64 K, 2–5 bar) is released from a small nozzle (∅ 1.0 mm or 1.25 mm) as a vertical upward jet into ambient air (293 K, 1 atm). The near-nozzle flow is under-expanded and passes through a Mach disk to produce a supersonic then subsonic jet. Downstream of the Mach disk the flow transitions to a self-similar turbulent free jet that is amenable to RANS simulation.

Key physical effects:

- **Buoyancy:** Cold, dense H2 initially falls against buoyancy; warm, low-density H2 rises. The net effect depends on whether momentum or buoyancy dominates (`Ri = g·D/U²·(ρ_a − ρ_0)/ρ_0`).
- **Strong density gradients:** ρ_H2(50K) ≈ 3.3 kg/m³; ρ_air(293K) ≈ 1.2 kg/m³. The density ratio drives entrainment and the centerline decay law.
- **Non-ideal Cp(T):** Para-H2 has a pronounced Cp peak (~16 000 J/kg/K at ~170 K) due to quantum rotational state transitions (ortho↔para conversion). Standard constant-Cp or JANAF polynomial assumptions fail in the cryogenic range.

---

## 2. Solver Selection: `rhoReactingBuoyantFoam`

OpenFOAM v2406 solver used: **`rhoReactingBuoyantFoam`**

| Property | rhoReactingBuoyantFoam | reactingFoam |
|----------|----------------------|--------------|
| Density formulation | ρ = p/(RT) via `rhoThermo` | ψ·p via `psiThermo` |
| Gravity / buoyancy | Yes — solves p_rgh = p − ρgh | No |
| Thermo factory | `rhoReactionThermo` | `psiReactionThermo` |
| Pressure equation | Modified (p_rgh GAMG) | Standard |
| Suitable for this case | Yes | No (lacks buoyancy) |

`rhoReactingBuoyantFoam` was chosen because:
1. Buoyancy is physically essential — the cryogenic plume initially has ρ > ρ_air and buoyancy forces compete with jet momentum.
2. The `rhoThermo` framework uses density directly (`ρ = p/RT`), making it the natural choice for large density-ratio multi-species flows.
3. The `rhoReactionThermo` factory table is extended by the user library (see Section 5) to register the `hPolynomial + perfectGas` combination needed for para-H2.

---

## 3. Notional Nozzle Approach

The physical nozzle is under-expanded: the exit pressure exceeds ambient, and a barrel shock + Mach disk form in the first few diameters. Resolving this shock structure accurately requires a very fine mesh and compressible shock-capturing schemes and is not the focus of this validation.

Instead, the **notional nozzle** (pseudo-source) approach replaces the near-nozzle shock cell with an equivalent subsonic inlet at the location of the Mach disk. This is the same approach used by Mohammadpour & Salehi (2024).

### Conservation equations

The notional nozzle conditions (subscript `nn`) are derived from the real nozzle exit (subscript `e`) by conserving mass, momentum, and energy while relaxing to ambient pressure:

```
ρ_nn · A_nn · U_nn = ṁ_e                    (mass)
ρ_nn · A_nn · U_nn² = ṁ_e · U_e + (p_e − p_a) · A_e   (momentum)
h_nn + U_nn²/2 = h_e + U_e²/2               (energy, adiabatic)
p_nn = p_a                                   (pressure equilibrium)
Y_H2,nn = 1.0                                (pure H2)
```

From these, T_nn and D_nn follow from the perfect-gas EoS and the continuity equation.

### Test 1 conditions (Hecht & Panda 2019, ∅ 1.0 mm, T_noz = 58 K, p_noz = 2.0 bar)

For Test 1, the nozzle exit is unchoked (Ma ≈ 0.98), so the Mach disk is negligible and the notional nozzle reduces to the physical nozzle exit:

| Parameter | Value | Notes |
|-----------|-------|-------|
| D_nn | 1.0 mm | = D_nozzle (unchoked) |
| T_nn | 43.9 K | From isentropic expansion to p_a |
| U_nn | 535.3 m/s | Giannissi (2021) Table 1 |
| Y_H2 | 1.0 | Pure hydrogen |
| p_nn | 101 325 Pa | Ambient |

> Note: For the choked tests (Test 3: p_noz = 4.0 bar), D_nn > D_nozzle and must be computed from the conservation equations above before updating the inlet BC.

---

## 4. Thermophysical Modelling

### 4.1 Thermophysical framework

```
thermoType
{
    type            heRhoThermo;
    mixture         multiComponentMixture;
    transport       sutherland;
    thermo          hPolynomial;
    energy          sensibleEnthalpy;
    equationOfState perfectGas;
    specie          specie;
}
```

- **`heRhoThermo`** — energy-based rho thermo; solves for sensible enthalpy h.
- **`multiComponentMixture`** — blends per-species thermo by mass fraction; requires `operator*` and `operator+` on the thermo type.
- **`hPolynomial`** — Cp(T) as a degree-7 polynomial; supports mixing operators and is compatible with `perfectGas` (see Section 4.3).
- **`perfectGas`** — ρ = p·W/(R·T); valid because reservoir pressures are moderate (≤ 5 bar) and departure from ideality is small once the flow reaches the notional nozzle plane.
- **`sutherland`** — μ(T) = As · T^(3/2) / (T + Ts); fitted separately for H2 and air.

### 4.2 Para-H2 Cp polynomial

Para-H2 Cp(T) cannot be approximated as a constant or modelled with a simple 2-coefficient JANAF expression over 50–350 K. It has a prominent peak near 170 K due to quantum rotational transitions.

Data source: CoolProp `parahydrogen` fluid, Leachman et al. (2009) EOS, sampled at 301 points over 50–350 K.

Degree-7 polynomial fit (T in Kelvin, Cp in J/kg/K):

```
Cp_H2(T) = 2.794912e+04
          − 8.466487e+02 · T
          + 1.459633e+01 · T²
          − 1.115357e−01 · T³
          + 4.402859e−04 · T⁴
          − 9.131228e−07 · T⁵
          + 8.928426e−10 · T⁶
          − 2.725380e−13 · T⁷
```

Fit quality: max error 0.93% at 50 K, mean error 0.19% over 50–350 K.

Sutherland transport for H2: As = 6.897×10⁻⁷, Ts = 97 K  
(μ(50 K) ≈ 1.66 µPa·s vs NIST 1.72 µPa·s, ~3% error)

### 4.3 Why `hPolynomialThermo` rather than `hTabulatedThermo`

Two fundamental incompatibilities prevent using `hTabulatedThermo` for this case:

1. **Missing mixing operators.** `multiComponentMixture` blends species thermo by calling `operator*` (scalar × thermo) and `operator+` (thermo + thermo). `hTabulatedThermo` does not define these operators — it was designed only for `pureMixture` (single-species). `hPolynomialThermo` defines both operators (polynomial coefficients are linearly blendable).

2. **EoS incompatibility.** `hTabulatedThermo::S()` internally calls `EoS::Sp(p, T)`, which `perfectGas` does not implement. `hPolynomialThermo::S()` instead calls `EoS::S(p, T)`, which `perfectGas` correctly implements as `−R·log(p/p_std)`.

### 4.4 Air Cp polynomial

Over 50–350 K, dry air Cp varies by only ~3% (~986–1016 J/kg/K). A degree-7 polynomial nonetheless is used for consistency:

```
Cp_air(T) = 9.799012e+02
           + 1.380391e−01 · T
           − 2.821240e−04 · T²
           + 4.572188e−07 · T³
           + 9.936515e−10 · T⁴
           − 4.392397e−12 · T⁵
           + 7.693694e−15 · T⁶
           − 5.613725e−18 · T⁷
```

Sutherland transport for air: As = 1.459×10⁻⁶, Ts = 111 K (White 2006).

---

## 5. User Thermophysical Library

The combination `heRhoThermo + multiComponentMixture + sutherlandTransport + hPolynomialThermo + perfectGas + specie` is not registered in OpenFOAM v2406's default `rhoReactionThermo` factory table. A user library registers it.

**Library location:**
```
$WM_PROJECT_DIR/../src/myThermophysicalModels/myReactionThermo/
```

**Key registration file:** `myRhoReactionThermo/myRhoReactionThermos.C`

The `makeReactionThermos` macro registers the type in both `rhoThermo` and `rhoReactionThermo` factory tables, making it available to `rhoReactingBuoyantFoam`.

**Compiled library:** `$FOAM_USER_LIBBIN/libmyReactionThermophysicalModels.so`

The case loads it at runtime via `controlDict`:
```cpp
libs ("libmyReactionThermophysicalModels.so");
```

---

## 6. Turbulence Modelling

**Model:** Standard k-ε (Launder & Spalding 1974) with buoyancy production terms  
**Implementation:** OpenFOAM `kEpsilon` with `simulationType RAS`

Buoyancy production is active by default in `rhoReactingBuoyantFoam` through the `Gb` term in the k-equation:
```
Gb = −μt/ρ · (∂ρ/∂xi) · gi / σT
```

Inlet turbulence boundary conditions:

| Patch | k BC | ε BC |
|-------|------|------|
| `inlet_fuel` | `turbulentIntensityKineticEnergyInlet` (I = 5%) | `turbulentMixingLengthDissipationRateInlet` (L = 0.07·D_nn) |
| `inlet_air` | `inletOutlet` | `inletOutlet` |

These yield k = 1.5·(I·U)² and ε = Cµ^(3/4)·k^(3/2)/L at the fuel inlet. The mixing length L = 0.07·D_nn follows the standard pipe-flow estimate for fully turbulent pipe flow.

---

## 7. Pressure Formulation

`rhoReactingBuoyantFoam` solves for the modified pressure:
```
p_rgh = p − ρ·g·h
```

This separates the hydrostatic component from the dynamic pressure, improving numerical conditioning in buoyancy-driven flows. Two pressure variables are present:

| Variable | Role |
|----------|------|
| `p` | Thermodynamic pressure (used in EoS, thermo) |
| `p_rgh` | Solved variable in momentum equation; p_rgh = p − ρgh |

Both `p` and `p_rgh` must appear in `fvSchemes/fluxRequired` and `fvSolution/solvers`.

---

## 8. Validation Targets

Extracted from Hecht & Panda (2019) via planar Raman scattering:

| Target | Correlation | Notes |
|--------|-------------|-------|
| Centerline H2 mole fraction decay | slope 0.277 in `1/X_H2 = K·(x/D)·√(ρ_a/ρ_0)` | Primary validation |
| Radial H2 mass fraction (Gaussian) | `Y/Y_cl = exp(−49η²)` where η = r/(x−x_0) | At multiple axial stations |
| Centerline temperature | half-width rate 0.062 | Coupled to concentration via mixing |

---

## 9. References

1. Hecht, E. S., & Panda, P. P. (2019). Mixing of trans and subcritical cryogenic hydrogen releases. *International Journal of Hydrogen Energy*, 44(17), 8823–8834.
2. Mohammadpour, J., & Salehi, F. (2024). OpenFOAM simulation of cryogenic hydrogen jets. *(reference simulation, methodological baseline)*
3. Leachman, J. W., et al. (2009). Fundamental equations of state for parahydrogen, normal hydrogen, and orthohydrogen. *J. Phys. Chem. Ref. Data*, 38(3), 721–748.
4. Giannissi, S. G. (2021). Numerical simulation of cryogenic hydrogen jets. *(notional nozzle inlet conditions)*
