/**
 * @file   ChargedSpacePointProxyInputMaker_module.cc
 * @brief  Test producer creating a few dummy space points and charges.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 20, 2017
 * 
 */

// LArSoft libraries
#include "lardata/ArtDataHelper/ChargedSpacePointCreator.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/PointCharge.h"

// framework libraries
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/Comment.h"

// Boost libraries
#include <boost/test/test_tools.hpp> // BOOST_CHECK()

// C/C++ standard libraries
#include <vector>
#include <memory> // std::make_unique()


namespace lar {
  namespace test {
    
    // -------------------------------------------------------------------------
    /**
    * @brief Creates some dummy space points and charge.
    * 
    * The produced space points and charges have completely dummy content.
    * They are implicitly associated and the amount of charge is as much as
    * the ID of the space point.
    * 
    * Configuration parameters
    * =========================
    * 
    * * *nPoints* (unsigned integer, default: 10): number of space points to
    *     generate
    * 
    */
    class ChargedSpacePointProxyInputMaker: public art::EDProducer {
        public:
      
      struct Config {
        using Name = fhicl::Name;
        using Comment = fhicl::Comment;
        
        fhicl::Atom<unsigned int> nPoints {
          Name("nPoints"),
          Comment("number of points to generate."),
          10U // default
          };
        
      }; // struct Config
      
      using Parameters = art::EDProducer::Table<Config>;
      
      explicit ChargedSpacePointProxyInputMaker(Parameters const& config);
      
      virtual void produce(art::Event& event) override;
      
        private:
      unsigned int nPoints; ///< Number of points to generate.

    };  // ChargedSpacePointProxyInputMaker

    // -------------------------------------------------------------------------
    
    
  } // namespace test
} // namespace lar


// -----------------------------------------------------------------------------
lar::test::ChargedSpacePointProxyInputMaker::ChargedSpacePointProxyInputMaker
  (Parameters const& config)
  : nPoints(config().nPoints())
{
  
  // declare production of recob::SpacePoint and recob::PointCharge collections:
  recob::ChargedSpacePointCollectionCreator::produces(*this);
  
} // ChargedSpacePointProxyInputMaker::ChargedSpacePointProxyInputMaker()

// -----------------------------------------------------------------------------
void lar::test::ChargedSpacePointProxyInputMaker::produce(art::Event& event) {
  
  recob::ChargedSpacePointCollectionCreator spacePoints(event, *this);
  
  BOOST_CHECK(spacePoints.empty());
  
  const double err[6U] = { 1.0, 0.0, 1.0, 0.0, 0.0, 1.0 };
  
  for (unsigned int iPoint = 0; iPoint < nPoints; ++iPoint) {
    BOOST_CHECK_EQUAL(spacePoints.size(), (std::size_t) iPoint);
    
    double const pos[3U]
      = { double(iPoint), double(2.0 * iPoint), double(4.0 * iPoint) };
    
    spacePoints.add(
      { pos, err, 1.0 /* chisq */, int(iPoint) /* id */ }, // space point
      { recob::PointCharge::Charge_t(iPoint) }                  // charge
      );
    
    mf::LogVerbatim("ChargedSpacePointProxyInputMaker")
      << "[#" << iPoint << "] point: " << spacePoints.lastSpacePoint()
      << " (ptr: " << spacePoints.lastSpacePointPtr()
      << "); charge: " << spacePoints.lastCharge()
      << " (ptr: " << spacePoints.lastChargePtr() << ")";
    
  } // for (iPoint)
  BOOST_CHECK_EQUAL(spacePoints.size(), (std::size_t) nPoints);
  
  mf::LogInfo("ChargedSpacePointProxyInputMaker")
    << "Produced " << spacePoints.size() << " points and charges.";
  
  spacePoints.put();
  BOOST_CHECK(spacePoints.empty());
  
} // lar::test::ChargedSpacePointProxyInputMaker::produce()

#if 0
// -----------------------------------------------------------------------------
void lar::test::ChargedSpacePointProxyInputMaker::produce(art::Event& event) {
  
  auto points = std::make_unique<std::vector<recob::SpacePoint>>();
  auto charges = std::make_unique<std::vector<recob::PointCharge>>();
  
  const double err[6U] = { 1.0, 0.0, 1.0, 0.0, 0.0, 1.0 };
  
  for (unsigned int iPoint = 0; iPoint < nPoints; ++iPoint) {
    
    //
    // point
    //
    double const pos[3U]
      = { double(iPoint), double(2.0 * iPoint), double(4.0 * iPoint) };
    points->emplace_back(pos, err, 1.0 /* chisq */, int(iPoint) /* id */);
    
    //
    // charge
    //
    charges->emplace_back(recob::PointCharge::Charge_t(iPoint));
    
    mf::LogVerbatim("ChargedSpacePointProxyInputMaker")
      << "[#" << iPoint << "] point: " << points->back()
      << "; charge: " << charges->back();
    
  } // for (iPoint)
  
  mf::LogInfo("ChargedSpacePointProxyInputMaker")
    << "Produced " << points->size() << " points and charges.";
  
  event.put(std::move(points));
  event.put(std::move(charges));
  
} // lar::test::ChargedSpacePointProxyInputMaker::produce()
#endif // 0

// -----------------------------------------------------------------------------
DEFINE_ART_MODULE(lar::test::ChargedSpacePointProxyInputMaker)

// -----------------------------------------------------------------------------
