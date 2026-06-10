/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021,2022 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "myPsiThermo.H"
#include "makeThermo.H"

#include "specie.H"
#include "perfectGas.H"
#include "PengRobinsonGas.H"
#include "hConstThermo.H"
#include "eConstThermo.H"
#include "janafThermo.H"
#include "sensibleEnthalpy.H"
#include "sensibleInternalEnergy.H"
#include "thermo.H"

#include "constTransport.H"
#include "sutherlandTransport.H"

#include "hPolynomialThermo.H"
#include "polynomialTransport.H"

#include "myHePsiThermo.H"
#include "pureZoneMixture.H"

#include "thermoPhysicsTypes.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/* * * * * * * * * * * * * * * * * Enthalpy-based * * * * * * * * * * * * * */

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    constTransport,
    sensibleEnthalpy,
    hConstThermo,
    perfectGas,
    specie
);

// Note: pureZoneMixture copies mixture model for every evaluation of cell
//       so can become expensive for complex models (e.g. with tables)

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleEnthalpy,
    hConstThermo,
    perfectGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleEnthalpy,
    janafThermo,
    perfectGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleEnthalpy,
    hConstThermo,
    PengRobinsonGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    polynomialTransport,
    sensibleEnthalpy,
    hPolynomialThermo,
    PengRobinsonGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    polynomialTransport,
    sensibleEnthalpy,
    janafThermo,
    PengRobinsonGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleEnthalpy,
    janafThermo,
    PengRobinsonGas,
    specie
);


/* * * * * * * * * * * * * * Internal-energy-based * * * * * * * * * * * * * */

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    constTransport,
    sensibleInternalEnergy,
    eConstThermo,
    perfectGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleInternalEnergy,
    eConstThermo,
    perfectGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    constTransport,
    sensibleInternalEnergy,
    hConstThermo,
    perfectGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleInternalEnergy,
    hConstThermo,
    perfectGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleInternalEnergy,
    janafThermo,
    perfectGas,
    specie
);

makeThermos
(
    myPsiThermo,
    myHePsiThermo,
    pureZoneMixture,
    sutherlandTransport,
    sensibleInternalEnergy,
    janafThermo,
    PengRobinsonGas,
    specie
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
