////////////////////////////////////////////////////////////////////////////
// \version 
//
// \brief Definition of data product to hold FlashMatch information
//
// \author bjpjones@mit.edu
//
////////////////////////////////////////////////////////////////////////////
#ifndef ANAB_FLASHMATCH_H
#define ANAB_FLASHMATCH_H

#include <vector>
#include <iosfwd>
#include <iostream>
#include <iomanip>

namespace anab {

  class FlashMatch{
  public:
    
    FlashMatch();
    
    double fChi2;
    int    fFlashID;
    int    fSubjectID;    
    bool   fInBeam;    

#ifndef __GCCXML__
  public:

    FlashMatch(double Chi2, int FlashID, int SubjectID, bool InBeam);

    friend std::ostream& operator << (std::ostream &o, FlashMatch const& a);

    const double& Chi2()        const; 
    const int&    FlashID()     const;
    const int&    SubjectID()   const;
    const bool&   InBeam()      const;
    
#endif
  };

}

#ifndef __GCCXML__

inline const double& anab::FlashMatch::Chi2()        const { return fChi2;        } 
inline const int&    anab::FlashMatch::FlashID()     const { return fFlashID;     }
inline const int&    anab::FlashMatch::SubjectID()     const { return fSubjectID;     }
inline const bool&   anab::FlashMatch::InBeam()     const { return fInBeam;     }


#endif

#endif //ANAB_FLASHMATCH
