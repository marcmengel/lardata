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
    : fSignalROI(0)
  {

  }

  //----------------------------------------------------------------------
  Wire::Wire(
    std::vector< std::pair< unsigned int, std::vector<float> > > sigROIlist,
    art::Ptr<raw::RawDigit> &rawdigit)
    : fSignalROI(sigROIlist)
    , fRawDigit(rawdigit)
  {

    art::ServiceHandle<geo::Geometry> geo;
    
    fView       = geo->View(rawdigit->Channel());
    fSignalType = geo->SignalType(rawdigit->Channel());
    fMaxSamples = rawdigit->NADC();

  }

  std::vector<float> Wire::Signal() const
  {
    // Return ROI signals in a zero padded vector of size that contains
    // all ROIs

    std::vector<float> sigTemp(fMaxSamples, 0.);
    if(fSignalROI.size() == 0) return sigTemp;
    
    for(unsigned int ir = 0; ir < fSignalROI.size(); ++ir) {
      unsigned int tStart = fSignalROI[ir].first;
      for(unsigned int ii = 0; ii < fSignalROI[ir].second.size(); ++ii) 
        sigTemp[tStart + ii] = fSignalROI[ir].second[ii];
    } // ir
    
    return sigTemp;
    
  } // Wire::Signal

  //----------------------------------------------------------------------
  Wire::~Wire()
  {

  }

}
////////////////////////////////////////////////////////////////////////

