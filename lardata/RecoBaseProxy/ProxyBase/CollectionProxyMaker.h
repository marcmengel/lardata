/**
 * @file   lardata/RecoBaseProxy/ProxyBase/CollectionProxyMaker.h
 * @brief  Infrastructure for the creation of a collection proxy.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * @see    lardata/RecoBaseProxy/ProxyBase/getCollection.h
 * 
 * This library is header-only.
 */

#ifndef LARDATA_RECOBASEPROXY_PROXYBASE_COLLECTIONPROXYMAKER_H
#define LARDATA_RECOBASEPROXY_PROXYBASE_COLLECTIONPROXYMAKER_H

// LArSoft libraries
#include "lardata/RecoBaseProxy/ProxyBase/CollectionProxy.h"
#include "larcorealg/CoreUtils/ContainerMeta.h" // util::collection_value_t, ...
#include "larcorealg/CoreUtils/MetaUtils.h" // util::always_true_type

// framework libraries
#include "canvas/Utilities/InputTag.h"

// C/C++ standard 
#include <vector>


namespace proxy {
  
  // --- BEGIN Collection proxy infrastructure ---------------------------------
  /// @addtogroup LArSoftProxyCollections
  /// @{
  
  //----------------------------------------------------------------------------
  /**
   * @brief Collection of data type definitions for collection proxies.
   * @tparam Proxy type of proxy the traits refer to
   * @tparam Selector specialization helper type
   * 
   * Expected traits:
   * * `main_collection_t`: type of the main data product collection
   * * `main_element_t`: type contained in the main data product collection
   * * `main_collection_proxy_t`: type wrapping the main data product collection
   * 
   * Note that the `Proxy` type is expected to be the same type as used in
   * `getCollection()` calls, and does not need to match the actual type of a
   * proxy collection.
   */
  template <typename Proxy, typename Selector = Proxy>
  struct CollectionProxyMakerTraits {
    static_assert
      (util::always_true_type<Proxy>(), "This class requires specialization.");
  };
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Class to assemble the required proxy.
   * @tparam CollProxy a type characterizing the produced proxy
   * 
   * This is a base class suitable for deriving `CollectionProxyMaker`,
   * which is used by `getCollection()` to create the requested proxy.
   * Deriving a specialization of `CollectionProxyMaker` from
   * `CollectionProxyMakerBase` provides some basic definitions and
   * functionality that might be tedious to copy.
   * 
   * In this, which can be considered as the default implementation of
   * `CollectionProxyMaker`, the template argument `CollProxy` is expected to
   * have the interface of `CollectionProxy`.
   * The arguments required are documented in the implementation of the `make()`
   * function. Note that the type of proxy returned does not need to be
   * `CollProxy`, but is in fact an instance of `proxy::CollectionProxy`.
   * 
   * @attention This class is not meant to be specialized.
   */
  template <typename CollProxy>
  struct CollectionProxyMakerBase {
    
    /// Traits of the collection proxy for the collection proxy maker.
    using traits_t = CollectionProxyMakerTraits<CollProxy>;
    
    /// Type of main collection proxy.
    using main_collection_proxy_t = typename traits_t::main_collection_proxy_t;
    
    /// Type returned by the main collection indexing operator.
    using main_element_t = typename traits_t::main_element_t;
    
    /// Type of element of the main collection.
    using main_collection_t = typename traits_t::main_collection_t;
    
    /**
     * @brief Creates and returns a collection proxy based on `CollProxy` and
     *        with the requested associated data.
     * @tparam Event type of the event to read the information from
     * @tparam WithArgs type of arguments for associated data
     * @param event event to read the information from
     * @param tag input tag of the main data product
     * @param withArgs optional associated objects to be included
     * @return a collection proxy from data as retrieved via `tag`
     * 
     * For each argument in `withArgs`, an action is taken. Usually that is to
     * add an association to the proxy.
     * 
     * Only a few associated data collections are supported:
     * * `withAssociated<Aux>()` (optional argument: hit-track association
     *     tag, as track by default): adds to the proxy an association to the
     *     `Aux` data product (see `withAssociated()`)
     * 
     */
    template <typename Event, typename... WithArgs>
    static auto make
      (Event const& event, art::InputTag const& tag, WithArgs&&... withArgs)
      {
        auto mainHandle = event.template getValidHandle<main_collection_t>(tag);
        return makeCollectionProxy(
          *mainHandle,
          withArgs.template createAuxProxyMaker<main_collection_proxy_t>
            (event, mainHandle, tag)...
          );
      } // make()
    
  }; // struct CollectionProxyMakerBase<>
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Class to assemble the required proxy.
   * @tparam CollProxy a type characterizing the produced proxy
   * 
   * This class is used by `getCollection()` to create the requested proxy.
   * The required interface for this class is:
   * * `make()`: a static method returning the collection proxy, matching the
   *   signature
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename... Args>
   *   auto make(Event const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *   where the first argument will be the event to read the information from.
   * 
   * A default implementation is provided as `proxy::CollectionProxyMakerBase`.
   * Specializations of `proxy::CollectionProxyMaker` may choose to derive from
   * `proxy::CollectionProxyMakerBase` as well, for convenience.
   */
  template <typename CollProxy>
  struct CollectionProxyMaker: CollectionProxyMakerBase<CollProxy> {};
  
  
  /// @}
  // --- END Collection proxy infrastructure -----------------------------------
  
  
  //----------------------------------------------------------------------------
  //--- specializations of CollectionProxyMakerTraits
  //----------------------------------------------------------------------------
  template <typename T>
  struct CollectionProxyMakerTraits<std::vector<T>> {
    
    /// Type of element of the main collection.
    using main_collection_t = std::vector<T>;
    
    /// Type returned by the main collection indexing operator.
    using main_element_t = util::collection_value_t<main_collection_t>;
    
    /// Type of main collection proxy.
    using main_collection_proxy_t
      = details::MainCollectionProxy<main_collection_t>;
    
  }; // CollectionProxyMakerTraits<std::vector<T>>
  
  
  template <typename MainColl>
  struct CollectionProxyMakerTraits<CollectionProxy<MainColl>> {
    /// Type of main collection proxy.
    using main_collection_proxy_t = CollectionProxy<MainColl>;
    
    /// Type returned by the main collection indexing operator.
    using main_element_t = typename main_collection_proxy_t::main_element_t;
    
    /// Type of element of the main collection.
    using main_collection_t
      = typename main_collection_proxy_t::main_collection_t;
  }; // CollectionProxyMakerTraits<CollectionProxy>
  
  
  // ---------------------------------------------------------------------------
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_PROXYBASE_COLLECTIONPROXYMAKER_H
