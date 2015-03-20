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
    
    double fTime;
    unsigned int    fTriggerType;
    unsigned int    fTriggerBits;    

#ifndef __GCCXML__
  public:

    T0(double Time, unsigned int TriggerType, unsigned int TriggerBits);

    friend std::ostream& operator << (std::ostream &o, T0 const& a);

    const double&          Time()        const; 
    const unsigned int&    TriggerType() const;
    const unsigned int&    TriggerBits() const;
    
#endif
  };

}

#ifndef __GCCXML__

inline const double&          anab::T0::Time()            const { return fTime;        } 
inline const unsigned int&    anab::T0::TriggerType()     const { return fTriggerType;     }
inline const unsigned int&    anab::T0::TriggerBits()     const { return fTriggerBits;     }

#endif

#endif //ANAB_T0
