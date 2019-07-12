////////////////////////////////////////////////////////////////////////
// \file PxHitConverter.cxx
//
// \brief conversion utulities from recob::Hit to PxHit
//
// \author andrzej.szelc@yale.edu, based on LarLite code by Kazu
//
//
////////////////////////////////////////////////////////////////////////

#include "TString.h"
#include "PxHitConverter.h"

#include "lardata/Utilities/GeometryUtilities.h"
#include "lardata/Utilities/UtilException.h"

namespace util {


   /// Generate: from 1 set of hits => 1 set of PxHits using indexes (association)


  void PxHitConverter::GeneratePxHit(std::vector<art::Ptr<recob::Hit>> const& hits,
				     std::vector<util::PxHit> &pxhits) const
  {

    if(!(hits.size())) throw UtilException(Form("Hit list empty! (%s)",__FUNCTION__));

    std::vector<unsigned int> hit_index;

    hit_index.clear();
    hit_index.reserve(hits.size());

    //generate full index
    for(unsigned int ix=0; ix<hits.size();++ix ) hit_index.push_back(ix);

    GeneratePxHit(hit_index,hits,pxhits);

  }


  void PxHitConverter::GenerateSinglePxHit(art::Ptr<recob::Hit> const& hit,
                                           util::PxHit &pxhit) const
  {
    pxhit = ToPxHit(hit);
  }


  PxHit PxHitConverter::HitToPxHit(recob::Hit const& hit) const
  {

    util::GeometryUtilities  gser;

    PxHit pxhit;
    pxhit.t      = hit.PeakTime() * gser.TimeToCm();
    pxhit.w      = hit.WireID().Wire * gser.WireToCm();
    pxhit.charge = hit.Integral();
    pxhit.sumADC = hit.SummedADC();
    pxhit.peak   = hit.PeakAmplitude();
    pxhit.plane  = hit.WireID().Plane;

    return pxhit;
  } // PxHitConverter::HitToPxHit(recob::Hit)



  /// Generate: from 1 set of hits => 1 set of PxHits using indexes (association)
  void PxHitConverter::GeneratePxHit(const std::vector<unsigned int>& hit_index,
				     const std::vector<art::Ptr<recob::Hit>> hits,
				     std::vector<util::PxHit> &pxhits) const
  {

    if(!(hit_index.size())) throw UtilException(Form("Hit list empty! (%s)",__FUNCTION__));

    pxhits.clear();
    pxhits.reserve(hit_index.size());

    for(auto const& index : hit_index) {

      auto const& hit = hits[index];

      PxHit h;
      GenerateSinglePxHit(hit,h);

      pxhits.push_back(h);
    }

  }

}// end namespace util
