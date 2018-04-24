/**
 * @file   lardata/RecoBaseProxy/ProxyBase/withParallelData.h
 * @brief  Interface to add auxiliary data from parallel collections to a proxy.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * @see    lardata/RecoBaseProxy/ProxyBase.h
 * 
 * This library is header-only.
 */

#ifndef LARDATA_RECOBASEPROXY_PROXYBASE_WITHPARALLELDATA_H
#define LARDATA_RECOBASEPROXY_PROXYBASE_WITHPARALLELDATA_H

// LArSoft libraries
#include "lardata/RecoBaseProxy/ProxyBase/ParallelDataProxyMaker.h"
#include "lardata/RecoBaseProxy/ProxyBase/WithAssociatedStructBase.h"
#include "larcorealg/CoreUtils/ContainerMeta.h" // util::collection_value_t, ...

// C/C++ standard 
#include <tuple>
#include <utility> // std::forward(), std::move()


namespace proxy {
  
  
  //----------------------------------------------------------------------------
  namespace details {
    
    template <typename Aux, typename ArgTuple, typename AuxTag = Aux>
    using WithParallelCollectionStruct = WithAssociatedStructBase<
      Aux,
      void, // no metadata concept for parallel collections
      ArgTuple,
      ParallelDataProxyMakerWrapper<Aux, AuxTag>::template maker_t,
      AuxTag
      >;
      
    
    template
      <typename Aux, typename ArgTuple, typename AuxColl, typename AuxTag = Aux>
    using WithWrappedParallelCollectionStruct = WithAssociatedStructBase<
      Aux,
      void, // no metadata concept for parallel collections
      ArgTuple,
      ParallelDataProxyMakerWrapper<Aux, AuxTag, AuxColl>::template maker_t,
      AuxTag
      >;
    
  } // namespace details
  
  
  // --- BEGIN Collection proxy infrastructure ---------------------------------
  /// @addtogroup LArSoftProxyCollections
  /// @{
  
  //----------------------------------------------------------------------------
  /// The same as `withParallelData()`, but it also specified a tag.
  template <typename Aux, typename AuxTag, typename... Args>
  auto withParallelDataAs(Args&&... args) {
    using ArgTuple_t = std::tuple<Args&&...>;
    ArgTuple_t argsTuple(std::forward<Args>(args)...);
    return details::WithParallelCollectionStruct<Aux, ArgTuple_t, AuxTag>
      (std::move(argsTuple));
  } // withParallelDataAs()
  
  //----------------------------------------------------------------------------
  /**
   * @brief Helper function to merge an auxiliary data product into the proxy.
   * @tparam Aux type of parallel data product requested
   * @tparam Args types of constructor arguments for parallel data proxy
   * @param args constructor arguments for the parallel data collection proxy
   * @return a temporary object that `getCollection()` knows to handle
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for merging a collection proxy to carry auxiliary data structured as a
   * collection parallel to the main collection.
   * The function also bridges the information required to create a proxy to
   * that auxiliary data.
   * 
   * This data will be tagged with the type `Aux`. To use a different type as
   * tag, use `withParallelDataAs()` instead, specifying the tag as second
   * template argument, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct MCS {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withParallelData<recob::TrackMomentum>(defaultMomTag),
   *   withParallelDataAs<recob::TrackMomentum, MCS>(MCSmomTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The first momentum association (`"defaultMomTag"`) will be accessed by
   * using the type `recob::TrackMomentum` as tag, while the second one will
   * be accessed by the `MCS` tag (which is better not be defined in a local
   * scope):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * for (auto&& track: tracks) {
   *   decltype(auto) trackMom = track.get<recob::TrackMomentum>();
   *   decltype(auto) trackMCSmom = track.get<MCS>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * The default implementation of parallel data proxy returns for each element
   * query an object with the same interface as the element of the parallel data
   * collection. In the previous examples, that would be a constant reference to
   * an object with `recob::TrackMomentum` interface.
   * 
   * 
   * Customization of the parallel data proxy
   * =========================================
   * 
   * To have a call like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = getCollection<SpecialTracks>
   *   (event, tag, withParallelData<recob::TrackMomentum>(momTag, "special"));
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * to create something different than the standard parallel data proxy,
   * one needs to specialize `proxy::ParallelDataProxyMaker`, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * namespace proxy {
   *   template <>
   *   struct ParallelDataProxyMaker<recob::Track, recob::TrackMomentum, SpecialTracks>
   *     : public ParallelDataProxyMakerBase<recob::Track, std::vector<recob::TrackMomentum>, recob::TrackMomentum>
   *   {
   *     
   *     template<typename Event, typename MainArgs>
   *     static auto make(
   *       Event const& event,
   *       MainArgs const&,
   *       art::InputTag assnTag,
   *       std::string quality = "default"
   *       )
   *       {
   *         ::SpecialTrackHitsProxy myAuxProxy;
   *         // ... make it, and make it right
   *         return myAuxProxy;
   *       }
   *     
   *   }; // struct ParallelDataProxyMaker<recob::Track, recob::TrackMomentum, SpecialTracks>
   *   
   * } // namespace proxy
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   */
  template <typename Aux, typename... Args>
  auto withParallelData(Args&&... args)
    { return withParallelDataAs<Aux, Aux>(std::forward<Args>(args)...); }
  
  
  //----------------------------------------------------------------------------
  /// Like `withParallelDataAs()`, but directly using the specified collection.
  template <typename AuxTag, typename AuxColl>
  auto wrapParallelDataAs(AuxColl const& auxColl) {
    std::tuple<AuxColl const&> args = { auxColl };
    return details::WithWrappedParallelCollectionStruct
      <util::collection_value_t<AuxColl>, decltype(args), AuxColl, AuxTag>
      (std::move(args));
  } // wrapParallelDataAs()
  /*
  /// Like `withParallelData()`, but directly using the specified collection.
  template <typename AuxTag, typename AuxColl>
  auto wrapParallelData(AuxColl const& auxColl)
    { return wrapParallelDataAs<AuxTag>(auxColl); }
  */
  /// Like `withParallelData()`, but directly using the specified collection.
  template <typename AuxColl>
  auto wrapParallelData(AuxColl const& auxColl)
    { return wrapParallelDataAs<util::collection_value_t<AuxColl>>(auxColl); }
  
  
  /// @}
  // --- END Collection proxy infrastructure -----------------------------------
  
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_PROXYBASE_WITHPARALLELDATA_H
