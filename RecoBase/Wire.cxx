////////////////////////////////////////////////////////////////////////
// $Id: Wire.cxx,v 1.10 2010/04/15 18:13:36 brebel Exp $
//
// Wire class
//
// brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#include "RecoBase/Wire.h"

#include "Geometry/CryostatGeo.h"
#include "Geometry/PlaneGeo.h"
#include "Geometry/TPCGeo.h"
#include "Geometry/Geometry.h"
#include "RawData/raw.h"

namespace recob{

  //----------------------------------------------------------------------
  Wire::Wire()
    : fSignal(0)
  {

  }

  //----------------------------------------------------------------------
  Wire::Wire(std::vector<float> siglist,
	     art::Ptr<raw::RawDigit> &rawdigit)
    : fSignal(siglist)
    , fRawDigit(rawdigit)
  {

    ///put the pedestal subtracted values of the raw adc's into siglist
    ///if siglist from initializer is empty
    if( fSignal.empty() ){
      std::vector<short> uncompressed(rawdigit->Samples());
      raw::Uncompress(rawdigit->fADC, uncompressed, rawdigit->Compression());
      for(size_t i = 0; i < uncompressed.size(); ++i)
	fSignal[i] = 1.*uncompressed[i] - rawdigit->GetPedestal();
      uncompressed.clear();
    }

    art::ServiceHandle<geo::Geometry> geo;
    
    fView       = geo->View(rawdigit->Channel());
    fSignalType = geo->SignalType(rawdigit->Channel());

  }

  //----------------------------------------------------------------------
  Wire::~Wire()
  {

  }

}
////////////////////////////////////////////////////////////////////////

