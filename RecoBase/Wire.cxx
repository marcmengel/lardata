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
    art::Ptr<raw::RawDigit> &rawdigit)
    : fSignalROI()
    , fRawDigit(rawdigit)
  {

    art::ServiceHandle<geo::Geometry> geo;
    
    fView       = geo->View(rawdigit->Channel());
    fSignalType = geo->SignalType(rawdigit->Channel());
    fMaxSamples = rawdigit->NADC();
    fSignalROI.resize(fMaxSamples); // "filled" with empty samples
  }

  //----------------------------------------------------------------------
  Wire::Wire
    (const RegionsOfInterest_t& sigROIlist, art::Ptr<raw::RawDigit> &rawdigit)
    : Wire(rawdigit)
  {
    fSignalROI = sigROIlist;
    fSignalROI.resize(fMaxSamples); // "filled" with empty samples
  }

  Wire::Wire
    (RegionsOfInterest_t&& sigROIlist, art::Ptr<raw::RawDigit> &rawdigit)
    : Wire(rawdigit)
  {
    fSignalROI = sigROIlist; // should use the move assignment
    fSignalROI.resize(fMaxSamples); // "filled" with empty samples
  }

  std::vector<float> Wire::Signal() const
  {
    return { fSignalROI.begin(), fSignalROI.end() };
#if 0 
    // *** untested code ***
    // Return ROI signals in a zero padded vector of size that contains
    // all ROIs

    std::vector<float> sigTemp(fMaxSamples, 0.);
    for(const auto& RoI: fSignalROI.get_ranges())
      std::copy(RoI.begin(), RoI.end(), sigTemp.begin() + RoI.begin_index());
    return sigTemp;
#endif // 0
    
  } // Wire::Signal

  //----------------------------------------------------------------------
  Wire::~Wire()
  {

  }

}
////////////////////////////////////////////////////////////////////////

