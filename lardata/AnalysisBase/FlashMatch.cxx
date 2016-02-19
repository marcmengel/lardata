////////////////////////////////////////////////////////////////////////
// \version $Id: 
//
// \brief Definition of FlashMatch analysis object
//
// \author bjpjones@mit.edu
////////////////////////////////////////////////////////////////////////

#include "lardata/AnalysisBase/FlashMatch.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace anab{

  //----------------------------------------------------------------------
  FlashMatch::FlashMatch() 
    : fChi2(0)
    , fFlashID(0)
    , fSubjectID(0)
    , fInBeam(false)
  {
  }

  //----------------------------------------------------------------------
  FlashMatch::FlashMatch(double Chi2,
			 int FlashID,
			 int SubjectID,
			 bool InBeam)
    : fChi2(Chi2)
    , fFlashID(FlashID)
    , fSubjectID(SubjectID)
    , fInBeam(InBeam)
  {

  }

  //----------------------------------------------------------------------
  // ostream operator.  
  //
  std::ostream& operator<< (std::ostream & o, FlashMatch const& a)
  {
    o << "Flash match with Chi2: "    << a.fChi2
      << "\n from flash: "            << a.fFlashID
      << "\n to object: "             << a.fSubjectID
      <<std::endl;
    

    return o;
  }
  
}
