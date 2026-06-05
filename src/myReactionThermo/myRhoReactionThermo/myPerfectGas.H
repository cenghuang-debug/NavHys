/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
Description
    Perfect gas EoS — identical to Foam::perfectGas but adds:
        Sp(p,T) = -R/p    (dS/dp at constant T for ideal gas)
    required by hTabulatedThermo when used with SpecieMixture
    (multiComponentMixture).  Use with equationOfState myPerfectGas in
    thermophysicalProperties.

\*---------------------------------------------------------------------------*/

#ifndef myPerfectGas_H
#define myPerfectGas_H

#include "autoPtr.H"
#include "thermodynamicConstants.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template<class Specie> class myPerfectGas;

template<class Specie>
inline myPerfectGas<Specie> operator+
(const myPerfectGas<Specie>&, const myPerfectGas<Specie>&);

template<class Specie>
inline myPerfectGas<Specie> operator*
(const scalar, const myPerfectGas<Specie>&);

template<class Specie>
inline myPerfectGas<Specie> operator==
(const myPerfectGas<Specie>&, const myPerfectGas<Specie>&);

template<class Specie>
Ostream& operator<<(Ostream&, const myPerfectGas<Specie>&);


/*---------------------------------------------------------------------------*\
                         Class myPerfectGas Declaration
\*---------------------------------------------------------------------------*/

template<class Specie>
class myPerfectGas
:
    public Specie
{

public:

    // Constructors

        inline myPerfectGas(const Specie& sp) : Specie(sp) {}

        myPerfectGas(const dictionary& dict) : Specie(dict) {}

        inline myPerfectGas(const word& name, const myPerfectGas& pg)
        : Specie(name, pg) {}

        inline autoPtr<myPerfectGas> clone() const
        { return autoPtr<myPerfectGas>::New(*this); }

        inline static autoPtr<myPerfectGas> New(const dictionary& dict)
        { return autoPtr<myPerfectGas>::New(dict); }


    // Member functions

        static word typeName()
        { return "myPerfectGas<" + word(Specie::typeName_()) + '>'; }

        static const bool incompressible = false;
        static const bool isochoric      = false;

        inline scalar rho(scalar p, scalar T) const
        { return p/(this->R()*T); }

        inline scalar H(scalar p, scalar T) const { return 0; }
        inline scalar Cp(scalar p, scalar T) const { return 0; }
        inline scalar E(scalar p, scalar T) const { return 0; }
        inline scalar Cv(scalar p, scalar T) const { return 0; }

        inline scalar S(scalar p, scalar T) const
        { return -this->R()*log(p/constant::thermodynamic::Pstd); }

        // Entropy-pressure derivative: (dS/dp)_T = -R/p  [J/(kg K Pa)]
        inline scalar Sp(scalar p, scalar T) const
        { return -this->R()/p; }

        inline scalar psi(scalar p, scalar T) const
        { return 1.0/(this->R()*T); }

        inline scalar Z(scalar p, scalar T) const { return 1; }

        inline scalar CpMCv(scalar p, scalar T) const { return this->R(); }

        void write(Ostream& os) const { Specie::write(os); }


    // Member operators

        inline void operator+=(const myPerfectGas& pg) { Specie::operator+=(pg); }
        inline void operator*=(const scalar s)          { Specie::operator*=(s); }


    // Friend operators

        friend myPerfectGas operator+ <Specie>
        (const myPerfectGas&, const myPerfectGas&);

        friend myPerfectGas operator* <Specie>
        (const scalar, const myPerfectGas&);

        friend myPerfectGas operator== <Specie>
        (const myPerfectGas&, const myPerfectGas&);

        friend Ostream& operator<< <Specie>
        (Ostream&, const myPerfectGas&);
};


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Specie>
inline myPerfectGas<Specie> operator+
(const myPerfectGas<Specie>& pg1, const myPerfectGas<Specie>& pg2)
{
    return myPerfectGas<Specie>
    (
        static_cast<const Specie&>(pg1) + static_cast<const Specie&>(pg2)
    );
}

template<class Specie>
inline myPerfectGas<Specie> operator*
(const scalar s, const myPerfectGas<Specie>& pg)
{
    return myPerfectGas<Specie>(s*static_cast<const Specie&>(pg));
}

template<class Specie>
inline myPerfectGas<Specie> operator==
(const myPerfectGas<Specie>& pg1, const myPerfectGas<Specie>& pg2)
{
    return myPerfectGas<Specie>
    (
        static_cast<const Specie&>(pg1) == static_cast<const Specie&>(pg2)
    );
}

template<class Specie>
Ostream& operator<<(Ostream& os, const myPerfectGas<Specie>& pg)
{
    os  << static_cast<const Specie&>(pg);
    return os;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
