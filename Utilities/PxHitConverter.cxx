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
	
   
  
  
    util::GeometryUtilities  gser;
    
     
     
    UChar_t plane = hit->WireID().Plane;
	 
    pxhit.t = hit->PeakTime() * gser.TimeToCm();
    pxhit.w = hit->WireID().Wire     * gser.WireToCm();

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
