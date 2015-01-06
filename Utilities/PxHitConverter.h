////////////////////////////////////////////////////////////////////////
// \file PxHitConverter.h
//
// \brief conversion utulities from recob::Hit to PxHit
//
// \author andrzej.szelc@yale.edu, based on LarLite code by Kazu
// 
//
////////////////////////////////////////////////////////////////////////
#ifndef UTIL_PXHITCONVERTER_H
#define UTIL_PXHITCONVERTER_H

//#include <TMath.h>
//#include <TLorentzVector.h>

#include "PxUtils.h"
#include "Geometry/Geometry.h"
#include "Utilities/LArProperties.h"
#include "Utilities/DetectorProperties.h"
#include "Utilities/UtilException.h"
//#include "time.h"

#include "RecoBase/Hit.h"
//#include "Geometry/CryostatGeo.h"
//#include "Geometry/PlaneGeo.h"
//#include "Geometry/WireGeo.h"
//#include "Geometry/TPCGeo.h"
//#include "SimpleTypesAndConstants/geo_types.h"
#include "art/Persistency/Common/Ptr.h" 

//#include <climits>
#include <iostream>
#include <vector>


///General LArSoft Utilities
namespace util{

  
  //class GeometryUtilities : public larlight::larlight_base {
  class PxHitConverter {

  public:

      public:
    
    /// Default constructor
    PxHitConverter(){};
    
    /// Default destructor
    virtual ~PxHitConverter(){};

//     /// Generate: from 1 set of hits => 1 CPAN using indexes (association)
//     void GenerateCPAN(const std::vector<unsigned int>& hit_index,
// 		      const larlite::event_hit* hits,
// 		      ClusterParamsAlg &cpan) const;
//     
//     /// Generate: CPAN vector from event storage by specifying cluster type
//     void GenerateCPAN(::larlite::storage_manager* storage,
// 		      const std::string &cluster_producer_name,
// 		      std::vector<cluster::ClusterParamsAlg> &cpan_v) const;
    
    /// Generate: from 1 set of hits => 1 set of PxHits using indexes (association)
    void GeneratePxHit(const std::vector<unsigned int>& hit_index,
		       const std::vector<art::Ptr<recob::Hit>> hits,
		       std::vector<util::PxHit> &pxhits) const;

      /// Generate: from 1 set of hits => 1 set of PxHits using using all hits
    void GeneratePxHit(const std::vector<art::Ptr<recob::Hit>> hits,
		       std::vector<util::PxHit> &pxhits) const;		       
		       
    void GenerateSinglePxHit(const art::Ptr<recob::Hit> hit,
		             util::PxHit &pxhits) const;	
		       

			     
		       
//     /// Generate: vector of PxHit sets from event storage by specifying cluster type
//     void GeneratePxHit(larlite::storage_manager* storage,
// 		       const std::string &cluster_producer_name,
// 		       std::vector<std::vector<larutil::PxHit> > &pxhits_v) const;
    
    }; // class PxHitConverter

} //namespace utils
#endif // UTIL_PXHITCONVERTER_H
