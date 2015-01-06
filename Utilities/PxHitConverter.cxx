////////////////////////////////////////////////////////////////////////
// \file PxHitConverter.cxx
//
// \brief conversion utulities from recob::Hit to PxHit
//
// \author andrzej.szelc@yale.edu, based on LarLite code by Kazu
// 
//
////////////////////////////////////////////////////////////////////////
#include "PxHitConverter.h"

namespace util {

  
   /// Generate: from 1 set of hits => 1 set of PxHits using indexes (association)
  void PxHitConverter::GeneratePxHit(const std::vector<art::Ptr<recob::Hit>> hits,
				std::vector<util::PxHit> &pxhits) const
  {

    if(!(hits.size())) throw UtilException(Form("Hit list empty! (%s)",__FUNCTION__));
    
    std::vector<unsigned int> hit_index;
    
    hit_index.clear();
    hit_index.reserve(hits.size());

    //generate full index
    for(unsigned int ix=0; ix<hits.size();++ix ) {
      
      hit_index.at(ix)=ix;
      
    }      

    GeneratePxHit(hit_index,hits,pxhits);
    
  }
  
  
  
   void PxHitConverter::GenerateSinglePxHit(const art::Ptr<recob::Hit> hit,
		             util::PxHit &pxhit) const
	{
	
    art::ServiceHandle<geo::Geometry>  geo;
    art::ServiceHandle<util::DetectorProperties> detp;
    art::ServiceHandle<util::LArProperties> larp;
    //util::GeometryUtilities  gser;
    double fWirePitch = geo->WirePitch(0,1,0);
    double fTimeTick=detp->SamplingRate()/1000.; 
    double fDriftVelocity=larp->DriftVelocity(larp->Efield(),larp->Temperature());
    
    double fWireToCm=fWirePitch;
    double fTimeToCm=fTimeTick*fDriftVelocity;
     
     
    UChar_t plane = hit->WireID().Plane;
	 
    pxhit.t = hit->PeakTime() * fTimeToCm;
    pxhit.w = hit->WireID().Wire     * fWireToCm;

    pxhit.charge = hit->Charge();
    pxhit.peak   = hit->Charge(true);
    pxhit.plane  = plane;  
	  
	  
			       
	}
		       
		       
		       
  
  
  
  
  
    
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
