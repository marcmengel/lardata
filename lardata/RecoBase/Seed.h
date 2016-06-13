////////////////////////////////////////////////////////////////////////////
// \version $Id: Seed.h,v 1.4 2010/06/10 16:21:31 antonm Exp $
//
// \brief 3D seed object for kalman tracking and
//           bezier tracking
//
// \author Ben Jones, MIT
//         bjpjones@mit.edu
//
////////////////////////////////////////////////////////////////////////////

#ifndef SEED_H
#define SEED_H

#include <iosfwd>

namespace recob {

  class SpacePoint;

  class Seed
  {
  public:
    // Basic constructors
    Seed();

  private:
    double fSeedPoint[3];
    double fSeedDirection[3];
    double fSeedPointError[3];
    double fSeedDirectionError[3];
    bool   fIsValid;
    
#ifndef __GCCXML__

  public:

    Seed(double* Pt, 
	 double * Dir, 
	 double* PtErr, 
	 double* DirErr);


    // Getters and setters
    void GetDirection(double* Dir, double* Err) const;
    void GetPoint(double*     Pt,  double* Err) const;
    double GetLength()                          const;

    void Print() const;

    void SetDirection(double* Dir, double* Err);
    void SetPoint(double *    Pt,  double* Err);


    double GetAngle(                Seed const & AnotherSeed ) const;
    double GetProjDiscrepancy (     Seed const & AnotherSeed ) const;
    double GetProjAngleDiscrepancy( Seed const & AnotherSeed ) const;
    double GetDistance(             Seed const & AnotherSeed ) const;

    Seed Reverse();
    void GetVectorBetween(          Seed const & AnotherSeed, double * xyz ) const;

    double GetDistanceFrom(         SpacePoint const & SomePoint) const;

    int GetPointingSign(            Seed const & AnotherSeed ) const;

    // Methods for errorless seeds
    Seed(double* Pt, double * Dir);
    void SetDirection(double* Dir);
    void SetPoint(double *    Pt);

    // Seed validity checks
    bool IsValid() const;
    void SetValidity(bool Validity);

    friend std::ostream& operator << (std::ostream& stream, Seed const& a);

    friend bool          operator <   (const Seed & a, const Seed & b);

#endif
  };

#ifndef __GCCXML__
  void CrossProd(double, double, double,
		 double, double, double,
		 double&, double&, double&);
#endif
}

#endif // SEED_H
