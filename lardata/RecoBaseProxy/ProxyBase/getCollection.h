/**
 * @file   lardata/RecoBaseProxy/ProxyBase/getCollection.h
 * @brief  Creation of a collection proxy.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * @see    lardata/RecoBaseProxy/ProxyBase.h
 * 
 * This library is header-only.
 */

#ifndef LARDATA_RECOBASEPROXY_PROXYBASE_GETCOLLECTION_H
#define LARDATA_RECOBASEPROXY_PROXYBASE_GETCOLLECTION_H

// LArSoft libraries
#include "lardata/RecoBaseProxy/ProxyBase/CollectionProxyMaker.h"

// C/C++ standard 
#include <utility> // std::forward()


/// Encloses LArSoft data product proxy objects and utilities.
/// @ingroup LArSoftProxies
namespace proxy {
  
  // --- BEGIN Collection proxy infrastructure ---------------------------------
  /// @ingroup LArSoftProxyCollections
  /// @{
  
  /**
   * @brief Creates a proxy to a data product collection.
   * @tparam CollProxy type of target main collection proxy
   * @tparam Event type of event to read data from
   * @tparam OptionalArgs type of optional arguments
   * @param event event to read data from
   * @param optionalArgs optional arguments for construction of the proxy
   * @return a collection proxy object
   * 
   * This function delivers a collection proxy related to `CollProxy`.
   * 
   * The @ref LArSoftProxyQuirks "type of proxy delivered is arbitrary"
   * and usually not `CollProxy`.
   * The type of the collection proxy must be explicitly specified, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = proxy::getCollection<proxy::Tracks>
   *   (event, tag, withAssociated<recob::Hits>());
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this case, two optional arguments are passed: the input tag to the main
   * collection, and then `withAssociated<recob::Hits>()`. The meaning of both
   * is decided depending on the collection proxy being created, but it's common
   * to have the first argument be the input tag to the main collection, as in
   * the example.
   * 
   * The collection proxy name is arbitrary, but it's custom to have it live in
   * `proxy` namespace and have the same name as the base object, made plural:
   * a proxy to a `recob::Track` collection data product will have a proxy
   * called `proxy::Tracks`.
   * 
   * Note that a proxy need to be explicitly supported in order to be available.
   * 
   * In practice, this function does very little apart from invoking the proper
   * `CollectionProxyMaker` class. Each proxy has its own, and there is where
   * the meaning of the optional arguments is assigned.
   * 
   * 
   * Customization
   * ==============
   * 
   * To control which type of collection proxy is produced for the type
   * `CollProxy`, the class `CollectionProxyMaker` may be specialised.
   * 
   */
  template <typename CollProxy, typename Event, typename... OptionalArgs>
  auto getCollection(Event const& event, OptionalArgs&&... optionalArgs)
    {
      return CollectionProxyMaker<CollProxy>::make
        (event, std::forward<OptionalArgs>(optionalArgs)...);
    }
  
  
  /// @}
  // --- END Collection proxy infrastructure -----------------------------------
  
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_PROXYBASE_GETCOLLECTION_H
