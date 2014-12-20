/** ****************************************************************************
 * @file   HitCreator.cxx
 * @brief  Helper functions to create a hit - implementation file
 * @date   December 19, 2014
 * @author petrillo@fnal.gov
 * @see    Hit.h HitCreator.h
 * 
 * ****************************************************************************/

// declaration header
#include "RecoBaseArt/HitCreator.h"

// C/C++ standard library
#include <utility> // std::move()
#include <algorithm> // std::accumulate()

// art libraries
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Persistency/Common/EDProductGetter.h"

// LArSoft libraries
#include "Geometry/Geometry.h"
// #include "RawData/RawDigit.h"
#include "RecoBase/Wire.h"
#include "RecoBase/Hit.h"
#include "Utilities/AssociationUtil.h"


/// Reconstruction base classes
namespace recob {
  
  //****************************************************************************
  //***  HitCreator
  //----------------------------------------------------------------------
  HitCreator::HitCreator(
    recob::Wire const&   wire,
    geo::WireID const&   wireID,
    raw::TDCtick_t       start_tick,
    raw::TDCtick_t       end_tick,
    float                rms,
    float                peak_time,
    float                sigma_peak_time,
    float                peak_amplitude,
    float                sigma_peak_amplitude,
    float                hit_integral,
    float                hit_sigma_integral,
    float                summedADC,
    short int            multiplicity,
    short int            local_index,
    float                goodness_of_fit,
    int                  dof,
    std::vector<float>&& signal
    ):
    hit(
      wire.Channel(),
      start_tick,
      end_tick,
      peak_time,
      sigma_peak_time,
      rms,
      peak_amplitude,
      sigma_peak_amplitude,
      summedADC,
      hit_integral,
      hit_sigma_integral,
      multiplicity,
      local_index,
      goodness_of_fit,
      dof,
      wire.View(),
      art::ServiceHandle<geo::Geometry>()->SignalType(wire.Channel()),
      wireID,
      std::move(signal)
      )
  {} // HitCreator::HitCreator(move)
  
  
  //----------------------------------------------------------------------
  HitCreator::HitCreator(
    recob::Wire const&   wire,
    geo::WireID const&   wireID,
    raw::TDCtick_t       start_tick,
    raw::TDCtick_t       end_tick,
    float                rms,
    float                peak_time,
    float                sigma_peak_time,
    float                peak_amplitude,
    float                sigma_peak_amplitude,
    float                hit_integral,
    float                hit_sigma_integral,
    short int            multiplicity,
    short int            local_index,
    float                goodness_of_fit,
    int                  dof,
    std::vector<float>&& signal
    ):
    HitCreator(
      wire, wireID, start_tick, end_tick,
      rms, peak_time, sigma_peak_time, peak_amplitude, sigma_peak_amplitude,
      hit_integral, hit_sigma_integral,
      std::accumulate(
        wire.SignalROI().begin() + start_tick,
        wire.SignalROI().begin() + end_tick,
        0.
        ), // sum of ADC counts between start_tick and end_tick
      multiplicity, local_index,
      goodness_of_fit, dof, std::move(signal)
      )
  {} // HitCreator::HitCreator(move)
  
  
  //----------------------------------------------------------------------
  HitCreator::HitCreator(
    recob::Wire const&        wire,
    geo::WireID const&        wireID,
    float                     rms,
    float                     peak_time,
    float                     sigma_peak_time,
    float                     peak_amplitude,
    float                     sigma_peak_amplitude,
    float                     hit_integral,
    float                     hit_sigma_integral,
    float                     summedADC,
    short int                 multiplicity,
    short int                 local_index,
    float                     goodness_of_fit,
    int                       dof,
    RegionOfInterest_t const& signal
    ):
    HitCreator(
      wire, wireID, signal.begin_index(), signal.end_index(),
      rms, peak_time, sigma_peak_time, peak_amplitude, sigma_peak_amplitude,
      hit_integral, hit_sigma_integral, summedADC, multiplicity, local_index,
      goodness_of_fit, dof, { signal.begin(), signal.end() }
      )
  {} // HitCreator::HitCreator(RoI; no summedADC; RoI)
  
  
  //----------------------------------------------------------------------
  HitCreator::HitCreator(
    recob::Wire const&        wire,
    geo::WireID const&        wireID,
    float                     rms,
    float                     peak_time,
    float                     sigma_peak_time,
    float                     peak_amplitude,
    float                     sigma_peak_amplitude,
    float                     hit_integral,
    float                     hit_sigma_integral,
    float                     summedADC,
    short int                 multiplicity,
    short int                 local_index,
    float                     goodness_of_fit,
    int                       dof,
    size_t                    iSignalRoI
    ):
    HitCreator(
      wire, wireID, rms, peak_time, sigma_peak_time, peak_amplitude, sigma_peak_amplitude,
      hit_integral, hit_sigma_integral, summedADC, multiplicity, local_index,
      goodness_of_fit, dof, wire.SignalROI().range(iSignalRoI)
      )
  {} // HitCreator::HitCreator(RoI; no summedADC; RoI index)
  
  
  
  //****************************************************************************
  //***  HitCollectionCreator
  //----------------------------------------------------------------------
  HitCollectionCreator::HitCollectionCreator(
    art::EDProducer& producer, art::Event& event,
    std::string instance_name /* = "" */,
    bool doWireAssns /* = true */, bool doRawDigitAssns /* = true */
    )
    : prod_instance(instance_name)
    , hits(new std::vector<recob::Hit>)
    , WireAssns
      (doWireAssns? new art::Assns<recob::Wire, recob::Hit>: nullptr)
    , RawDigitAssns
      (doRawDigitAssns? new art::Assns<raw::RawDigit, recob::Hit>: nullptr)
  {
    // get the product ID
    hit_prodId
      = producer.getProductID<std::vector<recob::Hit>>(event, prod_instance);
    
    // this must be run in the producer constructor...
  //  declare_products(producer, doWireAssns, doRawDigitAssns);
  } // HitCollectionCreator::HitCollectionCreator()
  
  
  //----------------------------------------------------------------------
  void HitCollectionCreator::declare_products(
    art::EDProducer& producer,
    std::string instance_name /* = "" */,
    bool doWireAssns /* = true */, bool doRawDigitAssns /* = true */
  ) {
    producer.produces<std::vector<recob::Hit>>(instance_name);
    
    // declare the other products we are creating (if any)
    if (doWireAssns)
      producer.produces<art::Assns<recob::Wire, recob::Hit>>(instance_name);
    if (doRawDigitAssns)
      producer.produces<art::Assns<raw::RawDigit, recob::Hit>>(instance_name);
    
  } // HitCollectionCreator::declare_products()
  
  
  //----------------------------------------------------------------------
  inline HitCollectionCreator::HitPtr_t HitCollectionCreator::CreatePtr
    (size_t index) const
  {
    return { hit_prodId, &((*hits)[index]), index };
  } // HitCollectionCreator::CreatePtr()
  
  
  //----------------------------------------------------------------------
  void HitCollectionCreator::emplace_back(
    recob::Hit&& hit,
    art::Ptr<recob::Wire> const& wire, art::Ptr<raw::RawDigit> const& digits
  ) {
    
    // add the hit to the collection
    hits->emplace_back(std::move(hit));
    
    // if no association is required, we are done
    if (!WireAssns && !RawDigitAssns) return;
    
    // art pointer to the hit we just created
    HitPtr_t hit_ptr(CreatePtrToLastHit());
    
    // association with wires
    if (WireAssns && wire.isNonnull())
      WireAssns->addSingle(wire, hit_ptr); // if it fails, it throws
    
    // association with wires
    if (RawDigitAssns && digits.isNonnull())
      RawDigitAssns->addSingle(digits, hit_ptr); // if it fails, it throws
    
  } // HitCollectionCreator::emplace_back(HitCreator)
  
  
  //----------------------------------------------------------------------
  void HitCollectionCreator::put_into(art::Event& event) {
    event.put(std::move(hits));
    if (WireAssns) event.put(std::move(WireAssns));
    if (RawDigitAssns) event.put(std::move(RawDigitAssns));
  } // HitCollectionCreator::put_into()
  
  
  //----------------------------------------------------------------------
} // namespace recob
