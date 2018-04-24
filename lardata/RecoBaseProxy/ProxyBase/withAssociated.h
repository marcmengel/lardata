/**
 * @file   lardata/RecoBaseProxy/ProxyBase/withAssociated.h
 * @brief  Functions to add associated data to a collection proxy.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * @see    lardata/RecoBaseProxy/ProxyBase.h
 * 
 * This library is header-only. It provides two classes of functions:
 * 
 * * `proxy::withAssociated()`: reads and parses an association from the event
 * * `proxy::wrapAssociated()`: parses an already existing association object
 * 
 */

#ifndef LARDATA_RECOBASEPROXY_PROXYBASE_WITHASSOCIATED_H
#define LARDATA_RECOBASEPROXY_PROXYBASE_WITHASSOCIATED_H

// LArSoft libraries
#include "lardata/RecoBaseProxy/ProxyBase/WithAssociatedStructBase.h"
#include "lardata/RecoBaseProxy/ProxyBase/AssociatedDataProxyMaker.h"
// #include "lardata/RecoBaseProxy/ProxyBase/AssociatedData.h"

// C/C++ standard libraries
#include <tuple>
#include <utility> // std::forward(), std::move()
  

namespace proxy {
  
  //----------------------------------------------------------------------------
  namespace details {
    
    //--------------------------------------------------------------------------
    /**
     * @class WithAssociatedStruct
     * @brief Helper to create associated data proxy.
     * @tparam Aux type of data associated to the main one
     * @tparam Metadata type of metadata of the association
     * @tparam ArgTuple type of arguments required for the creation of proxy
     * @tparam AuxTag tag for the associated data (default: as `Aux`)
     * 
     * This class stores user arguments for the construction of a proxy to
     * associated data of type `Aux` and with metadata `Metadata`.
     * It can use that information plus some additional one to create the
     * associated data itself. This additional information is provided by
     * `getCollection()`.
     * 
     * The association will be identified by type `AuxTag`.
     * 
     * This is not a customization point: to have a custom associated data
     * produced, specialize `proxy::AssociatedDataProxyMaker` class
     */
    template <
      typename Aux, typename Metadata,
      typename ArgTuple, typename AuxTag = Aux
      >
    using WithAssociatedStruct = WithAssociatedStructBase<
      Aux,
      Metadata,
      ArgTuple,
      AssociatedDataProxyMakerWrapper<Aux, Metadata, AuxTag>::template maker_t,
      AuxTag
      >;
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
  // --- BEGIN Collection proxy infrastructure ---------------------------------
  /// @addtogroup LArSoftProxyCollections
  /// @{
  
  //----------------------------------------------------------------------------
  /// The same as `withAssociated()`, but it also specified a tag for the data
  /// and one for the metadata.
  template <typename Aux, typename Metadata, typename AuxTag, typename... Args>
  auto withAssociatedMetaAs(Args&&... args) {
    using ArgTuple_t = std::tuple<Args&&...>;
    ArgTuple_t argsTuple(std::forward<Args>(args)...);
    return details::WithAssociatedStruct<Aux, Metadata, ArgTuple_t, AuxTag>
      (std::move(argsTuple));
  } // withAssociatedMetaAs()
  
  
  //----------------------------------------------------------------------------
  /// The same as `withAssociated()`, but it also specified a tag for the data.
  template <typename Aux, typename AuxTag, typename... Args>
  auto withAssociatedAs(Args&&... args)
    {
      return withAssociatedMetaAs<Aux, void, AuxTag>
        (std::forward<Args>(args)...);
    }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Helper function to merge associated data.
   * @tparam Aux type of associated data requested
   * @tparam Metadata type of associated metadata requested
   * @tparam Args types of constructor arguments for associated data collection
   * @param args constructor arguments for the associated data collection
   * @return a temporary object that `getCollection()` knows to handle
   * @see withAssociated(), withAssociatedAs(), withAssociatedMetaAs()
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for the delivered collection proxy to carry auxiliary data. The associated
   * data is normally extracted from an _art_ association
   * `art::Assns<Main, Aux, Metadata>`, where `Main` is the main type of the
   * proxy collection. If no metadata is required, `Metadata` can be set to
   * `void`, or `withAssociated()` can be used instead.
   * 
   * The function also transfers the information required to create a proxy to
   * that auxiliary data.
   * 
   * This data will be tagged with the type `Aux`. To use a different type as
   * tag, use `withAssociatedAs()` or `withAssociatedMetaAs()` instead,
   * specifying the tag as second template argument, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct DubiousClusters {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withAssociatedMeta<recob::Cluster, void>(defaultClusterTag),
   *   withAssociatedMetaAs<recob::Cluster, void, DubiousClusters>
   *     (maybeClusterTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * or, equivalently (because we asked for no metadata):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct DubiousClusters {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withAssociated<recob::Cluster>(defaultClusterTag),
   *   withAssociatedAs<recob::Cluster, DubiousClusters>(maybeClusterTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The first cluster association (`"defaultClusterTag"`) will be accessed by
   * using the type `recob::Cluster` as tag, while the second one will be
   * accessed by the `DubiousClusters` tag (which is better not be defined in a
   * local scope):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * for (auto&& track: tracks) {
   *   decltype(auto) clusters = track.get<recob::Clusters>();
   *   decltype(auto) maybeClusters = track.get<DubiousClusters>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * 
   * Customization of the association proxy
   * =======================================
   * 
   * To have a call like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = getCollection<SpecialTracks>
   *   (event, tag, withAssociatedMeta<recob::Hit, void>(hitAssnTag, "special"));
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * create something different than the standard association proxy, specialize
   * `proxy::AssociatedDataProxyMaker`, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * namespace proxy {
   *   template <>
   *   struct AssociatedDataProxyMaker
   *     <recob::Track, recob::Hit, void, SpecialTracks>
   *     : public AssociatedDataProxyMakerBase
   *       <recob::Track, recob::Hit, void, SpecialTracks>
   *   {
   *     
   *     template<typename Event, typename MainArgs>
   *     static auto make(
   *       Event const& event,
   *       MainArgs const&,
   *       art::InputTag assnTag,
   *       std::string quality
   *       )
   *       {
   *         ::SpecialTrackHitsProxy myAuxProxy;
   *         // ... make it, and make it right
   *         return myAuxProxy;
   *       }
   *     
   *   }; // struct AssociatedDataProxyMaker<..., SpecialTracks>
   *   
   * } // namespace proxy
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * (the `void` template type signifies the association has no metadata).
   * 
   * 
   * Technical details
   * ==================
   * 
   * The main purpose of this function and the related `WithAssociatedStruct`
   * class is to save the user from specifying the main type the auxiliary data
   * is associated with, when using it as `getCollection()` argument:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = getCollection<proxy::Tracks>
   *   (event, tag, withAssociated<recob::Hit>(hitAssnTag));
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * While parsing the `withAssociated()` argument (or any argument), the
   * information of which is the proxy collection type (`proxy::Tracks` in the
   * example) is not known. In principle, to fully define the association, two
   * template arguments are needed, e.g.
   * `withAssociated<recob::Track, recob::Hit>(hitAssnTag)`.
   * The class `WithAssociatedStruct` holds the information of which associated
   * type is requested (`recob::Hit`) and the information needed to create a
   * proxy to such association (all arguments, here just `hitAssnTag`).
   * The function `getCollection()` will have this object as argument, and when
   * executing will be able to supply the missing information, that
   * `recob::Track` is the main data product element we are associating to.
   */
  template <typename Aux, typename Metadata, typename... Args>
  auto withAssociatedMeta(Args&&... args)
    {
      return withAssociatedMetaAs<Aux, Metadata, Aux>
        (std::forward<Args>(args)...); 
    }
  
  
  /**
   * @brief Helper function to merge associated data with no metadata.
   * @tparam Aux type of associated data requested
   * @tparam Args types of constructor arguments for associated data collection
   * @param args constructor arguments for the associated data collection
   * @return a temporary object that `getCollection()` knows to handle
   * @see withAssociatedMeta(), withAssociatedMetaAs()
   * 
   * This function is equivalent to `withAssociatedMeta()` but with the request
   * of no associated metadata (`Metadata` be `void`).
   */
  template <typename Aux, typename... Args>
  auto withAssociated(Args&&... args)
    { return withAssociatedMeta<Aux, void>(std::forward<Args>(args)...); }
  
  
  //----------------------------------------------------------------------------
  /// Like `withAssociatedAs()`, but directly using the specified association.
  template <typename AuxTag, typename Assns>
  auto wrapAssociatedAs(Assns const& assns)
    {
      using Aux_t = typename Assns::right_t;
      using Metadata_t = lar::util::assns_metadata_t<Assns>;
      return
        details::WithAssociatedStruct<Aux_t, Metadata_t, std::tuple<>, AuxTag>
        ({});
    } // wrapAssociatedAs()
  
  
  /// Like `withAssociated()`, but directly using the specified association.
  template <typename AuxTag, typename Assns>
  auto wrapAssociated(Assns const& assns)
    { return wrapAssociatedAs<AuxTag>(assns); }
  
  /// Like `withAssociated()`, but directly using the specified association.
  template <typename Assns>
  auto wrapAssociated(Assns const& assns)
    { return wrapAssociatedAs<typename Assns::right_t>(assns); }
  
  
  /// @{
  // --- END Collection proxy infrastructure -----------------------------------
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_PROXYBASE_WITHASSOCIATED_H
