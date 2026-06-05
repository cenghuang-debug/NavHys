# NavHys: CFD Validation of Low-Temperature and Cryogenic H₂ Release

Gas-phase only (no liquid phase). OpenFOAM v2406 on WSL2.

## Goals

1. Publish a peer-reviewed validation/benchmarking paper for low-temperature H₂ release CFD.
2. Apply the validated model to real-world scenarios: Tank Connection Space (TCS) and vent mast release.

## Current Scope: Two Validation Cases

### Hecht & Panda (2019) — Sandia, cryogenic (`Hecht_Panda_2019.pdf`)
- **Conditions**: 50–64 K, 2–5 bar, nozzle ∅ 1.0 and 1.25 mm, vertical upward jet, 9 conditions
- **Data**: Planar Raman scattering — 2D H₂ mole fraction and temperature fields
- **Validation targets**: centerline H₂ decay (slope 0.277), radial Gaussian profiles (`Y/Y_cl = exp(−49η²)`), temperature decay (half-width rate 0.062)
- **Reference sim**: Mohammadpour & Salehi (2024) — OpenFOAM k-ε, notional nozzle at 2 mm downstream of Mach disk, ~500–560K hex cells. Use as methodological baseline; do not simply repeat it.

### Gong et al. (2022) — low-temperature (`Gong2022.pdf`)
- **Conditions**: 200–300 K, 0.5 MPa, nozzle ∅ 1.0–2.0 mm, 9 cases (3 T × 3 D)
- **Data**: Centerline H₂ concentration (10 point sensors, 5 cm spacing)
- **Validation targets**: centerline decay law (`1/C_H2 = 0.000188·(x/D)·√(ρ_a/ρ_0) + 0.0064`), safety distances at 4 %, 11 %, 1 % H₂

## Solver & Modelling Approach

- **Solver candidates**: `rhoReactingBuoyantFoam`, `reactingFoam`, `rhoPimpleFoam`
- **Turbulence**: k-ε baseline; buoyancy production terms enabled
- **Notional nozzle**: bypass under-expanded near-nozzle region; inlet conditions from isentropic expansion + mass/momentum conservation
- **Thermophysics**: JANAF polynomials; heRhoThermo with H₂/air species mixing

## Validation Metrics

| Quantity | Source |
|----------|--------|
| Centerline H₂ mole fraction decay | Hecht & Panda (2019), Gong (2022) |
| Radial H₂ mass fraction (Gaussian fit) | Hecht & Panda (2019) |
| Centerline temperature decay | Hecht & Panda (2019) |
| 2D concentration and temperature contours | Hecht & Panda (2019) |
| Safety distances (4 %, 1 % H₂) | Gong (2022) |

## Directory Structure

```
NavHys/
├── CLAUDE.md
├── literature/
│   ├── Hecht_Panda_2019.pdf
│   ├── Gong2022.pdf
│   ├── Mohammadpour_Salehi_2024.pdf  # reference sim
│   └── Friedrich2012.pdf             # future work
├── cases/
│   ├── Hecht_Panda_2019/
│   ├── Gong2022/
│   ├── TCS/                          # application (future)
│   └── VentMast/                     # application (future)
└── postProcessing/
    ├── scripts/
    └── figures/
```

## Future Work
- Friedrich et al. (2012): 35–65 K, 0.7–3.5 MPa, KIT ICESAFE — extend to higher-pressure cryogenic conditions once current validation is established.
- TCS and vent mast application cases.
