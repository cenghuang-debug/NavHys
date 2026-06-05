/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
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

Description
    Registers
      heRhoThermo<multiComponentMixture<sutherlandTransport<
        hPolynomialThermo<perfectGas<specie>,8>,sensibleEnthalpy>>>
    into rhoReactionThermo.

    hPolynomialThermo is used instead of hTabulatedThermo because it defines
    operator* and operator+ required by multiComponentMixture for species
    blending, and calls EoS::S() (not EoS::Sp()) so perfectGas is compatible.

\*---------------------------------------------------------------------------*/

#include "makeReactionThermo.H"

#include "rhoReactionThermo.H"
#include "heRhoThermo.H"

#include "specie.H"
#include "perfectGas.H"
#include "hPolynomialThermo.H"
#include "sensibleEnthalpy.H"
#include "thermo.H"
#include "sutherlandTransport.H"
#include "multiComponentMixture.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// heRhoThermo + multiComponentMixture + sutherland + hPolynomial(8) + perfectGas
makeReactionThermos
(
    rhoThermo,
    rhoReactionThermo,
    heRhoThermo,
    multiComponentMixture,
    sutherlandTransport,
    sensibleEnthalpy,
    hPolynomialThermo,
    perfectGas,
    specie
);

} // End namespace Foam

// ************************************************************************* //
