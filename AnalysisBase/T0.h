////////////////////////////////////////////////////////////////////////////
// \version 
//
// \brief Definition of data product to hold T0 information
//
// \author k.warburton@sheffield.ac.uk
//
////////////////////////////////////////////////////////////////////////////
#ifndef ANAB_T0_H
#define ANAB_T0_H

#include <vector>
#include <iosfwd>
#include <iostream>
#include <iomanip>

namespace anab {

  class T0{
  public:
    
    T0();
    
    double       fTime;
    unsigned int fTriggerType;
    int          fTriggerBits;
    double       fTriggerConfidence;
    int          fID;

#ifndef __GCCXML__
  public:

    T0(double Time, unsigned int TriggerType, int TriggerBits, double TriggerConfidence, int ID=-1);

    friend std::ostream& operator << (std::ostream &o, T0 const& a);

    const double&          Time()              const; 
    const unsigned int&    TriggerType()       const;
    const int&             TriggerBits()       const;
    const double&          TriggerConfidence() const;
    const int&             ID()                const;
    
#endif
  };

}

#ifndef __GCCXML__

inline const double&          anab::T0::Time()              const { return fTime;              } /// Time in ns
inline const unsigned int&    anab::T0::TriggerType()       const { return fTriggerType;       } /// Type of trigger used. 0 - Muon Counters, 1 - Photon Detectors, 2 - Monte Carlo Truth
inline const int&             anab::T0::TriggerBits()       const { return fTriggerBits;       } /// An identifier for the Muon track / Flash / MCParticle used in matching.
inline const double&          anab::T0::TriggerConfidence() const { return fTriggerConfidence; } /// Confidence with which this T0 is known.
inline const int&             anab::T0::ID()                const { return fID;                } /// Current size of T0 data product.

#endif

#endif //ANAB_T0
