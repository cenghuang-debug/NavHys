# Cryogenic H2 Dispersion Model — Run Guide

**Solver:** `rhoReactingBuoyantFoam`  
**OpenFOAM:** v2406 on WSL2  
**Case directory:** `NavHys/cases/Hecht_Panda_2019/cryogenic_H2_test_03/`

---

## Prerequisites

- OpenFOAM v2406 sourced (`. $WM_PROJECT_DIR/etc/bashrc`)
- `$WM_PROJECT_DIR` points to the OpenFOAM v2406 installation
- The user library has been compiled (Step 1 below)

---

## Step 1: Compile the User Thermophysical Libraries

Two libraries are required and must be compiled in order. Both live in `NavHys/src/` — the repo is self-contained.

**Option A — compile both at once (recommended):**
```bash
cd <path-to-NavHys>/src
./Allwmake
```

**Option B — compile individually:**
```bash
# 1. Base psi-thermo layer (myBasic must come first)
cd <path-to-NavHys>/src/myBasic
wmake

# 2. Reaction thermo registration (depends on myBasic)
cd <path-to-NavHys>/src/myReactionThermo
wmake
```

**Expected outputs:**
```
$FOAM_USER_LIBBIN/libmyFluidThermophysicalModels.so    ← from myBasic
$FOAM_USER_LIBBIN/libmyReactionThermophysicalModels.so ← from myReactionThermo
```

To verify:
```bash
ls $FOAM_USER_LIBBIN/libmy*.so
```

`libmyReactionThermophysicalModels.so` registers:
```
heRhoThermo<
  multiComponentMixture<
    sutherlandTransport<
      hPolynomialThermo<perfectGas<specie>, 8>,
      sensibleEnthalpy>>>
```
in both `rhoThermo` and `rhoReactionThermo` factory tables via `makeReactionThermos`.

**If recompilation is needed:**
```bash
cd <path-to-NavHys>/src
wclean myBasic && wclean myReactionThermo
./Allwmake
```

---

## Step 2: Navigate to the Case

```bash
cd /home/chenhu/OpenFOAM/chenhu-v2406/run/NavHys/cases/Hecht_Panda_2019/cryogenic_H2_test_03
```

---

## Step 3: Build the Mesh

The mesh is a 2D axisymmetric wedge (5° total, ±2.5°) in the r-z plane.

**Domain dimensions:**

| Parameter | Value |
|-----------|-------|
| Outer radius | 50 mm |
| Domain height | 140 mm |
| Wedge half-angle | 2.5° |
| Nozzle diameter | 1.0 mm |
| Nozzle stub height | 2.0 mm |

**Radial resolution:** 0.25 mm (fine, 0–5 mm) / 1.0 mm (coarse, 5–50 mm)  
**Axial resolution:** 0.25 mm (fine, 0–14 mm) / 1.0 mm (coarse, 14–140 mm)

Mesh generation uses four tools in sequence:

```bash
# 1. Clean previous run artifacts
rm -rf 0 constant/polyMesh

# 2. Build base wedge mesh
blockMesh > log.blockMesh

# 3. Carve out nozzle stub cavity
topoSet -dict system/topoSetDict_new_domain > log.topoSetDict_new_domain
subsetMesh -overwrite new_domain -patch internalWalls > log.subsetMesh

# 4. Split the combined inlet into inlet_fuel and inlet_air patches
topoSet -dict system/topoSetDict_inlets > log.topoSetDict_inlets
createPatch -overwrite > log.createPatch

# 5. Set nozzle stub walls to type 'wall' (sed patches lines 50-51 in boundary)
sed -i '50s/empty/wall/' constant/polyMesh/boundary
sed -i '51s/empty/wall/' constant/polyMesh/boundary

# 6. Copy initial fields
cp -r 0.orig/ 0/

# 7. Quality check and renumber for solver efficiency
checkMesh > log.checkMesh
renumberMesh -overwrite > log.renumberMesh
```

Or equivalently, run the provided `Allrun` script (first uncomment the solver line — see Step 5):
```bash
./Allrun
```

**Check mesh quality:**
```bash
grep -E "Max|non-orthogonality|skewness" log.checkMesh
```
Target: max non-orthogonality < 70°, max skewness < 4.

---

## Step 4: Review Boundary Conditions

Initial field files are in `0.orig/`. They are copied to `0/` by `Allrun`.

### Patches summary

| Patch | Type | Description |
|-------|------|-------------|
| `inlet_fuel` | patch | Notional nozzle plane (pure H2 jet) |
| `inlet_air` | patch | Ambient co-flow annulus |
| `outlet` | patch | Top boundary |
| `wallPipe` | wall | Outer domain wall |
| `internalWalls` | wall | Nozzle tube exterior |
| `front` / `back` | wedge | Wedge symmetry planes |

### Key inlet conditions (Test 1: T_noz = 58 K, p_noz = 2.0 bar, unchoked)

| Field | `inlet_fuel` | `inlet_air` |
|-------|-------------|------------|
| U | `fixedValue (0 0 535.3)` m/s | `fixedValue (0 0 0.3)` m/s |
| T | `fixedValue 43.9` K | `fixedValue 293.15` K |
| p_rgh | `fixedFluxPressure` | `fixedFluxPressure` |
| H2 | `fixedValue 1.0` | `fixedValue 0.0` |
| air | `fixedValue 0.0` | `fixedValue 1.0` |
| k | `turbulentIntensityKineticEnergyInlet` (I=5%) | `inletOutlet` |
| epsilon | `turbulentMixingLengthDissipationRateInlet` (L=7×10⁻⁵ m) | `inletOutlet` |

> For other test conditions, update `U`, `T` at `inlet_fuel`, and `mixingLength` in `0.orig/epsilon` before running.

---

## Step 5: Run the Solver

Uncomment the solver line in `Allrun` (line 29), or run directly:

```bash
rhoReactingBuoyantFoam > log.rhoReactingBuoyantFoam
```

To run in the background and monitor progress:
```bash
rhoReactingBuoyantFoam > log.rhoReactingBuoyantFoam &
tail -f log.rhoReactingBuoyantFoam
```

**Key solver settings (`system/controlDict`):**

| Parameter | Value | Rationale |
|-----------|-------|-----------|
| `endTime` | 0.001 s | ~190 jet flow-through times |
| `deltaT` | 1×10⁻⁷ s | Initial; Co-limited near inlet |
| `maxCo` | 0.5 | Limits step near high-velocity inlet |
| `adjustTimeStep` | yes | Adaptive time-stepping |
| `writeInterval` | 0.001 s | One write at end |

**Time-step guidance:** The courant-limited time step near the 535 m/s inlet on a 0.25 mm cell is ~5×10⁻⁷ s. Expect ~2000–4000 steps per millisecond of simulation time.

### Monitoring convergence

Key residuals to watch in the log:
```
GAMG:  Solving for p_rgh   — should drop to < 1e-6 per outer iteration
PBiCGStab: Solving for h   — enthalpy residual
PBiCGStab: Solving for H2  — species residual
```

Healthy first-timestep output looks like:
```
Time = 1e-07
GAMG: Solving for p_rgh, Initial residual = 0.032, Final residual = 1.3e-08
...
T range: 43.9 to 293.15    ← confirms cryogenic inlet is active
```

---

## Step 6: Post-Processing

### Extract line data (automatic via `controlDict` functions)

Five `singleGraph` function objects write data at every write time:

| Object | Geometry |
|--------|----------|
| `singleGraph_z` | Centerline, z = 0–140 mm |
| `singleGraph_r10` | Radial profile at z = 10 mm |
| `singleGraph_r20` | Radial profile at z = 20 mm |
| `singleGraph_r30` | Radial profile at z = 30 mm |
| `singleGraph_r40` | Radial profile at z = 40 mm |

Output location: `postProcessing/singleGraph_z/`, `postProcessing/singleGraph_r10/`, etc.  
Files contain columns: `x` (or `r`), `H2`, `T`, `U_z`, etc.

### ParaView visualisation

Open `open.foam` in ParaView, or convert to VTK:
```bash
foamToVTK
```
VTK files appear in `VTK/`.

### Validation comparison

See `postProcessing/scripts/` (or the future Jupyter notebook) for:
- Centerline H2 mole fraction vs `1/X_H2 = 0.277·(x/D)·√(ρ_a/ρ_0) + const`
- Radial Gaussian fit: `Y/Y_cl = exp(−49η²)`
- Temperature decay half-width slope 0.062

---

## Step 7: Clean Up

```bash
./Allclean
```

This removes `0/`, `constant/polyMesh/`, log files, and `postProcessing/`.

---

## Adapting to Other Test Conditions

The table below lists key inlet parameters that must be changed per test condition. Edit `0.orig/U`, `0.orig/T`, and `0.orig/epsilon` (mixingLength), then re-run from Step 3.

| Test | T_noz (K) | p_noz (bar) | T_nn (K) | U_nn (m/s) | D_nn (mm) | mixingLength (m) |
|------|-----------|-------------|----------|------------|-----------|-----------------|
| 1 | 58 | 2.0 | 43.9 | 535.3 | 1.00 | 7.0×10⁻⁵ |
| 2 | *(TBD)* | *(TBD)* | *(TBD)* | *(TBD)* | *(TBD)* | *(TBD)* |
| 3 | 53 | 4.0 | *(compute)* | *(compute)* | *(compute via notional nozzle)* | 0.07 × D_nn |

For choked tests (p_noz ≥ ~1.9 bar at 50 K), compute D_nn, T_nn, U_nn from the notional nozzle conservation equations described in `docs/methodology.md` Section 3 before updating the BCs.

---

## Troubleshooting

### `Unknown rhoReactionThermo type`
The user library is not loaded or not compiled.  
Fix: compile the library (Step 1) and confirm `libs ("libmyReactionThermophysicalModels.so");` is in `controlDict`.

### `Invalid wall function specification — patch inlet_air must be wall`
`inlet_air` is `type patch`, not `type wall`. Wall function BCs (`nutkWallFunction`, `kqRWallFunction`, etc.) are invalid on it.  
Fix: use `calculated`, `inletOutlet`, or `fixedValue` on `inlet_air`. Wall functions are only on `wallPipe` and `internalWalls`.

### `flux requested but p_rgh not specified in fluxRequired`
Fix: add `p_rgh;` to `fluxRequired {}` in `system/fvSchemes`.

### `p_rgh not found in fvSolution`
Fix: add entries for `p_rgh` and `p_rghFinal` in `system/fvSolution/solvers`.

### Solver diverges / T goes negative
- Reduce `maxCo` to 0.2 for the first few timesteps.
- Check that `inlet_fuel` temperature is > 0 K (T_nn = 43.9 K for Test 1).
- Verify `hRef` in `constant/hRef` is set (used by buoyant solver as reference height).
