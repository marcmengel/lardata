/**
 * @file   lardata/RecoBaseProxy/ProxyBase.h
 * @brief  Base utilities for the implementation of data product facades.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * 
 * This library is header-only.
 */

#ifndef LARDATA_RECOBASEPROXY_PROXYBASE_H
#define LARDATA_RECOBASEPROXY_PROXYBASE_H

// LArSoft libraries
#include "lardata/RecoBaseProxy/ProxyBase/ParallelData.h"
#include "lardata/RecoBaseProxy/ProxyBase/OneTo01Data.h"
#include "lardata/RecoBaseProxy/ProxyBase/AssociatedData.h"
#include "lardata/RecoBaseProxy/ProxyBase/AssnsNodeAsTuple.h"
#include "lardata/RecoBaseProxy/ProxyBase/AssnsTraits.h"
#include "lardata/RecoBaseProxy/ProxyBase/MainCollectionProxy.h"
#include "lardata/RecoBaseProxy/ProxyBase/CollectionProxyElement.h"
#include "lardata/RecoBaseProxy/ProxyBase/CollectionProxy.h"
#include "lardata/Utilities/CollectionView.h"
#include "lardata/Utilities/TupleLookupByTag.h" // util::getByTag(), ...
#include "larcorealg/CoreUtils/ContainerMeta.h" // util::collection_value_t, ...
#include "larcorealg/CoreUtils/DebugUtils.h" // lar::debug::demangle()

// framework libraries
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Utilities/InputTag.h"

// C/C++ standard 
#include <vector>
#include <string> // std::to_string()
#include <tuple> // std::tuple_element_t<>, std::get()
#include <iterator> // std::distance(), ...
#include <algorithm> // std::min()
#include <utility> // std::forward()
#include <stdexcept> // std::runtime_error, std::logic_error
#include <type_traits> // std::is_same<>, std::enable_if_t<>, ...
#include <limits> // std::numeric_limits<>
#include <cstdlib> // std::size_t
#include <cassert>



/// Encloses LArSoft data product proxy objects and utilities.
/// @ingroup LArSoftProxies
namespace proxy {
  
  
  //----------------------------------------------------------------------------
  namespace details {
    
    //--------------------------------------------------------------------------
    // forward declarations
    //--------------------------------------------------------------------------
    template <
      typename Aux,
      typename Metadata,
      typename ArgTuple,
      template <typename CollProxy> class ProxyMaker,
      typename AuxTag = Aux
      >
    class WithAssociatedStructBase;
    
    template <typename Aux, typename Metadata = void, typename AuxTag = Aux>
    struct AssociatedDataProxyMakerWrapper;
    
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

    template <typename Aux, typename Metadata = void, typename AuxTag = Aux>
    struct OneTo01DataProxyMakerWrapper;
    
    template <
      typename Aux, typename Metadata, typename ArgTuple,
      typename AuxTag = Aux
      >
    using WithOneTo01AssociatedStruct = WithAssociatedStructBase<
      Aux,
      Metadata,
      ArgTuple,
      OneTo01DataProxyMakerWrapper<Aux, Metadata, AuxTag>::template maker_t,
      AuxTag
      >;

    template <typename Aux, typename AuxTag, typename AuxColl = void>
    struct ParallelDataProxyMakerWrapper;
    
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
    
    
    //--------------------------------------------------------------------------
    template <
      typename AuxProxyColl,
      typename Aux = util::collection_value_t<AuxProxyColl>,
      typename Tag = Aux
      >
    struct ProxyAsParallelData;
    
    template <
      typename AuxProxy,
      typename ArgTuple,
      typename AuxTag = AuxProxy
      >
    class WithProxyAsAuxStructBase;
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
  //----------------------------------------------------------------------------
  // forward declarations
  template <typename CollProxy, typename Event, typename... OptionalArgs>
  auto getCollection(Event const&, OptionalArgs&&...);
  
  
  //----------------------------------------------------------------------------
  /**
   * @defgroup LArSoftProxiesAssociatedData Associated data infrastructure
   * @ingroup  LArSoftProxyCustom
   * @brief Infrastructure for support of associated data.
   * 
   * Associated data is auxiliary data connected to the main data via _art_
   * associations.
   * The following associated data are currently supported:
   * * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association"
   *   implicitly supporting also one-to-any (one-to-one, one-to-zero/or/one) in
   *   a non-optimized way
   * * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero-or-one) sequential association"
   * * @ref LArSoftProxyDefinitionParallelData "parallel data product", implicit
   *     one-to-one associations
   * 
   * @{
   */
  
  //----------------------------------------------------------------------------
  //--- one-to-(zero/one) associations
  //----------------------------------------------------------------------------
  /**
   * @brief Processes and returns an one-to-(zero/one) associated data object.
   * @tparam Tag the tag labelling this associated data
   *             (if omitted: second type of the association: `right_t`)
   * @tparam Assns type of association to be processed
   * @param assns association object to be processed
   * @param minSize minimum number of entries in the produced association data
   * @return a new `OneTo01Data` filled with associations from `tag`
   * 
   * The content of the association object must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero or one) sequential association".
   * The `Assns` type is expected to be a `art::Assns` instance. At least,
   * the `Assns` type is required to have `left_t` and `right_t` definitions
   * representing respectively the main data type and the associated one, and
   * to respond to `begin()` and `end()` functions. The iterated object must
   * also respond to `std::get<0>()` with a `art::Ptr<left_t>` and to
   * `std::get<1>()` with a `art::Ptr<right_t>`.
   * 
   * Elements in the main collection not associated with any object will present
   * an invalid _art_ pointer (`isNull()` true). If there is information for
   * less than `minSize` main objects, more records will be added to mark the
   * missing objects as not associated to anything.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * art::Assns<recob::Track, recob::Vertex> trackVertexAssns;
   * // ...
   * auto assData = makeOneTo01dataFrom(assns);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * will have `assData` tagged as `recob::Vertex`.
   */
  template <typename Tag, typename Assns>
  auto makeOneTo01dataFrom(Assns const& assns, std::size_t minSize = 0)
    { return proxy::makeOneTo01data<Tag>(assns, minSize); }
  
  template <typename Assns>
  auto makeOneTo01dataFrom(Assns const& assns, std::size_t minSize = 0)
    { return proxy::makeOneTo01data(assns, minSize); }
  
  /**
   * @brief Creates and returns an one-to-(zero/one) associated data object.
   * @tparam Main type of main object to be associated
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Metadata type of metadata in the association
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Event type of event to read associations from
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @param minSize minimum number of entries in the produced association data
   * @return a new `OneTo01Data` filled with associations from `tag`
   * @see `makeOneTo01dataFrom(Assns, std::size_t)`
   * 
   * The association being retrieved must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero or one) sequential association".
   * 
   * Two template types must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeOneTo01dataFrom<recob::Track, recob::Vertex>(event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <
    typename Main, typename Aux, typename Metadata, typename Tag, typename Event
    >
  auto makeOneTo01dataFrom
    (Event const& event, art::InputTag tag, std::size_t minSize = 0);
  
  template <typename Main, typename Aux, typename Metadata, typename Event>
  auto makeOneTo01dataFrom
    (Event const& event, art::InputTag tag, std::size_t minSize = 0)
    {
      return makeOneTo01dataFrom<Main, Aux, Metadata, Aux, Event>
        (event, tag, minSize);
    }
  
  /**
   * @brief Creates and returns an one-to-(zero/one) associated data object.
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Handle type of handle to the main collection object
   * @tparam Event type of event to read associations from
   * @param handle handle to the main collection object
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @return a new `OneTo01Data` wrapping the information in `assns`
   * @see `makeAssociatedDataFrom(Event const&, art::InputTag, std::size_t)`
   * 
   * This function operates like
   * `makeOneTo01dataFrom(Event const&, art::InputTag, std::size_t)`, but it
   * extracts the information about the type of main object and the minimum
   * number of them from a handle.
   * The handle object is expected to behave as a smart pointer to a
   * collection of elements of the associated type.
   * 
   * One template type must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeOneTo01dataFrom<recob::Vertex>(handle, event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <
    typename Aux, typename Metadata, typename Tag,
    typename Handle, typename Event
    >
  auto makeOneTo01dataFrom
    (Handle&& handle, Event const& event, art::InputTag tag);
  
  template <typename Aux, typename Metadata, typename Handle, typename Event>
  auto makeOneTo01dataFrom
    (Handle&& handle, Event const& event, art::InputTag tag)
    {
      return makeOneTo01dataFrom<Aux, Metadata, Aux, Handle, Event>
        (std::forward<Handle>(handle), event, tag);
    }
  
  
  /**
   * @brief Creates and returns an one-to-(zero/one) associated data object.
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam MainColl type of the main collection object
   * @tparam Assns type of the association object
   * @param mainColl the main collection object
   * @param assns association data object
   * @return a new `OneTo01Data` wrapping the information in `assns`
   * @see `makeOneTo01dataFrom(Assns const&, std::size_t)`
   * 
   * This function operates like
   * `makeOneTo01dataFrom(Assns const&, std::size_t)`, where the size is
   * extracted from the main data collection.
   */
  template <typename Tag, typename MainColl, typename Assns>
  auto makeOneTo01dataFrom(MainColl const& mainColl, Assns const& assns)
    { return proxy::makeOneTo01data<Tag>(assns, mainColl.size()); }
  
  template <typename MainColl, typename Assns>
  auto makeOneTo01dataFrom(MainColl const& mainColl, Assns const& assns)
    { return proxy::makeOneTo01data<typename Assns::right_t>(mainColl, assns); }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates an one-to-(zero-or-one) wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam Metadata type of metadata coming with the association
   * @tparam AuxTag tag labelling this association
   * 
   * Usually, `AuxTag` is also the type of datum (element) to associate to
   * ("right").
   * 
   * This class works as a base class for `OneTo01DataProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template <
    typename Main, typename Aux, typename Metadata = void,
    typename AuxTag = Aux
    >
  struct OneTo01DataProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum ("left").
    using main_element_t = Main;
    
    /// Type of the auxiliary associated datum ("right").
    using aux_element_t = Aux;
    
    /// Type of associated metadata.
    using metadata_t = Metadata;
    
    /// Type of associated data proxy being created.
    using aux_collection_proxy_t = details::OneTo01Data
       <main_element_t, aux_element_t, metadata_t, data_tag>;
    
    /// Type of _art_ association being used as input.
    using assns_t = typename aux_collection_proxy_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param mainArgs an object describing the main data product
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The `mainArgs` object is of an arbitrary type that must be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
      {
        return createFromTag
          (event, std::forward<Handle>(mainHandle), art::InputTag(mainArgs));
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param auxInputTag the tag of the association to be read
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make(
      Event const& event, Handle&& mainHandle,
      MainArgs const&, art::InputTag auxInputTag
      )
      {
        return
          createFromTag(event, std::forward<Handle>(mainHandle), auxInputTag);
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from (unused)
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param handle handle to the main data collection
     * @param assns the associations to be wrapped
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs, typename Assns>
    static auto make
      (Event const&, Handle&& handle, MainArgs const&, Assns const& assns)
      {
        static_assert(
          std::is_convertible<typename Assns::right_t, aux_element_t>(),
          "Improper right type for one-to-(zero-or-one) association."
          );
        return proxy::makeOneTo01data<data_tag>(assns, handle->size());
      }
    
    
      private:
    template<typename Event, typename Handle>
    static auto createFromTag
      (Event const& event, Handle&& mainHandle, art::InputTag auxInputTag)
      {
        return makeOneTo01dataFrom
          <main_element_t, aux_element_t, metadata_t, data_tag>
          (event, auxInputTag, mainHandle->size());
      }
    
  }; // struct OneTo01DataProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an one-to-(zero-or-one) wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam Metadata type of metadata in the association
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @see `withZeroOrOne()`
   * This class is (indirectly) called when using `proxy::withZeroOrOne()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the proxy to the data
   * association between the main data type and an auxiliary one.
   * The interface required by `withZeroOrOne()` includes:
   * * a static `make()` method creating and returning the associated data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the associated proxy (coming from
   *   `withZeroOrOne()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized (see `withAssociated()` for an example
   * of the general procedure).
   * The default implementation just wraps a one-to-(zero-or-one)
   * `art::Assns<Main, Aux>` data product fulfilling
   * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero or one) sequential association"
   * requirement.
   * 
   * The last template argument is designed for specialization of associations
   * in the context of a specific proxy type.
   */
  template <
    typename Main, typename Aux, typename Metadata,
    typename CollProxy, typename Tag = Aux
    >
  class OneTo01DataProxyMaker
    : public OneTo01DataProxyMakerBase<Main, Aux, Metadata, Tag>
  {
    //
    // Note that this implementation is here only to document how to derive
    // a OneTo01DataProxyMaker (specialization) from
    // OneTo01DataProxyMakerBase. It's just mirroring the base class.
    //
    using base_t = OneTo01DataProxyMakerBase<Main, Aux, Metadata, Tag>;
    
      public:
    
    /// Type of the main datum ("left").
    using typename base_t::main_element_t;
    
    /// Type of the auxiliary associated datum ("right").
    using typename base_t::aux_element_t;
    
    /// Type of metadata in the association.
    using typename base_t::metadata_t;
    
    /// Type of associated data proxy being created.
    using typename base_t::aux_collection_proxy_t;
    
    /// Type of _art_ association being used as input.
    using typename base_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of data product handle
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @tparam Args optional single type (`art::InputTag` required)
     * @param event the event to read associations from
     * @param mainHandle handle of the main collection data product
     * @param margs an object describing the main data product
     * @param args input tag for associated data, if different from main
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * This implementation requires `margs` object to be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... Args>
    static auto make(
      Event const& event, Handle&& mainHandle, MainArgs const& margs,
      Args&&... args
      )
      {
        return base_t::make(
          event,
          std::forward<Handle>(mainHandle),
          margs,
          std::forward<Args>(args)...
          );
      }
    
  }; // struct OneTo01DataProxyMaker<>
  
  
  //----------------------------------------------------------------------------
  //--- one-to-many associations
  //----------------------------------------------------------------------------
  /**
   * @brief Processes and returns an associated data object.
   * @tparam Tag the tag labelling this associated data
   *             (if omitted: second type of the association: `right_t`)
   * @tparam Assns type of association to be processed
   * @param assns association object to be processed
   * @param minSize minimum number of entries in the produced association data
   * @return a new `AssociatedData` filled with associations from `tag`
   * 
   * The content of the association object must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association".
   * The `Assns` type is expected to be a `art::Assns` instance. At least,
   * the `Assns` type is required to have `left_t` and `right_t` definitions
   * representing respectively the main data type and the associated one, and
   * to respond to `begin()` and `end()` functions. The iterated object must
   * also respond to `std::get<0>()` with a `art::Ptr<left_t>` and to
   * `std::get<1>()` with a `art::Ptr<right_t>`.
   * 
   * Elements in the main collection not associated with any object will be
   * recorded as such. If there is information for less than `minSize` main
   * objects, more records will be added to mark the missing objects as not
   * associated to anything.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * art::Assns<recob::Track, recob::Hit> trackHitAssns;
   * // ...
   * auto assData = makeAssociatedDataFrom(assns);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * will have `assData` tagged as `recob::Hit`.
   */
  template <typename Tag, typename Assns>
  auto makeAssociatedDataFrom(Assns const& assns, std::size_t minSize = 0)
    { return proxy::makeAssociatedData<Tag>(assns, minSize); }
  
  template <typename Assns>
  auto makeAssociatedDataFrom(Assns const& assns, std::size_t minSize = 0)
    { return makeAssociatedDataFrom<typename Assns::right_t>(assns, minSize); }
  
  /**
   * @brief Creates and returns an associated data object.
   * @tparam Main type of main object to be associated
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Metadata type of metadata in the association (if omitted: `void`)
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Event type of event to read associations from
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @param minSize minimum number of entries in the produced association data
   * @return a new `AssociatedData` filled with associations from `tag`
   * 
   * The association being retrieved must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association".
   * 
   * Elements in the main collection not associated with any object will be
   * recorded as such. If there is information for less than `minSize` main
   * objects, more records will be added to mark the missing objects as not
   * associated to anything.
   * 
   * Two template types must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeAssociatedDataFrom<recob::Track, recob::Hit>(event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <
    typename Main, typename Aux, typename Metadata, typename Tag,
    typename Event
    >
  auto makeAssociatedDataFrom
    (Event const& event, art::InputTag tag, std::size_t minSize = 0);
  
  template <typename Main, typename Aux, typename Metadata, typename Event>
  auto makeAssociatedDataFrom
    (Event const& event, art::InputTag tag, std::size_t minSize = 0)
    {
      return makeAssociatedDataFrom<Main, Aux, Metadata, Aux, Event>
        (event, tag, minSize);
    }
  
  template <typename Main, typename Aux, typename Event>
  auto makeAssociatedDataFrom
    (Event const& event, art::InputTag tag, std::size_t minSize = 0)
    {
      return makeAssociatedDataFrom<Main, Aux, void, Aux, Event>
        (event, tag, minSize);
    }
  
  /**
   * @brief Creates and returns an associated data object.
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Metadata type of metadata in the association (if omitted: `void`)
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Handle type of handle to the main collection object
   * @tparam Event type of event to read associations from
   * @param handle handle to the main collection object
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @return a new `AssociatedData` filled with associations from `tag`
   * @see `makeAssociatedDataFrom(Event const&, art::InputTag, std::size_t)`
   * 
   * This function operates like
   * `makeAssociatedDataFrom(Event const&, art::InputTag, std::size_t)`, but it
   * extracts the information about the type of main object and the minimum
   * number of them from a handle.
   * The handle object is expected to behave as a smart pointer to a
   * collection of elements of the associated type.
   * 
   * One template type must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeAssociatedDataFrom<recob::Hit>(handle, event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <
    typename Aux, typename Metadata, typename Tag,
    typename Handle, typename Event
    >
  auto makeAssociatedDataFrom
    (Handle&& handle, Event const& event, art::InputTag tag);
  
  template <typename Aux, typename Metadata, typename Handle, typename Event>
  auto makeAssociatedDataFrom
    (Handle&& handle, Event const& event, art::InputTag tag)
    {
      return makeAssociatedDataFrom<Aux, Metadata, Aux, Handle, Event>
        (std::forward<Handle>(handle), event, tag);
    }
  
  template <typename Aux, typename Handle, typename Event>
  auto makeAssociatedDataFrom
    (Handle&& handle, Event const& event, art::InputTag tag)
    {
      return makeAssociatedDataFrom<Aux, void, Aux, Handle, Event>
        (std::forward<Handle>(handle), event, tag);
    }
  
  
  /**
   * @brief Creates and returns an associated data object.
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam MainColl type of the main collection object
   * @tparam Assns type of the association object
   * @param mainColl the main collection object
   * @param assns association data object
   * @return a new `AssociatedData` wrapping the information in `assns`
   * @see `makeAssociatedDataFrom(Assns const&, std::size_t)`
   * 
   * This function operates like
   * `makeAssociatedDataFrom(Assns const&, std::size_t)`, where the size is
   * extracted from the main data collection.
   */
  template <typename Tag, typename MainColl, typename Assns>
  auto makeAssociatedDataFrom(MainColl const& mainColl, Assns const& assns)
    { return proxy::makeAssociatedData<Tag>(assns, mainColl.size()); }
  
  template <typename MainColl, typename Assns>
  auto makeAssociatedDataFrom(MainColl const& mainColl, Assns const& assns)
    { return makeAssociatedDataFrom<typename Assns::right_t>(mainColl, assns); }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates an associated data wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam AuxTag tag labelling this association
   * 
   * Usually, `AuxTag` is also the type of datum (element) to associate to
   * ("right").
   * 
   * This class works as a base class for `AssociatedDataProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template
    <typename Main, typename Aux, typename Metadata, typename AuxTag = Aux>
  struct AssociatedDataProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum ("left").
    using main_element_t = Main;
    
    /// Type of the auxiliary associated datum ("right").
    using aux_element_t = Aux;
    
    /// Type of metadata in the association.
    using metadata_t = Metadata;
    
    /// Type of associated data proxy being created.
    using aux_collection_proxy_t = details::AssociatedData
      <main_element_t, aux_element_t, metadata_t, data_tag>;
    
    /// Type of _art_ association being used as input.
    using assns_t = typename aux_collection_proxy_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param mainArgs an object describing the main data product
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The `mainArgs` object is of an arbitrary type that must be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
      {
        return createFromTag
          (event, std::forward<Handle>(mainHandle), art::InputTag(mainArgs));
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param auxInputTag the tag of the association to be read
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make(
      Event const& event, Handle&& mainHandle,
      MainArgs const&, art::InputTag auxInputTag
      )
      {
        return
          createFromTag(event, std::forward<Handle>(mainHandle), auxInputTag);
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from (unused)
     * @tparam Handle type of handle to the main data product (unused)
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param assns the associations to be wrapped
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs, typename Assns>
    static auto make
      (Event const&, Handle&&, MainArgs const&, Assns const& assns)
      {
        static_assert(
          std::is_convertible<typename Assns::right_t, aux_element_t>(),
          "Improper right type for association."
          );
        return makeAssociatedDataFrom<data_tag>(assns);
      }
    
    
      private:
    template<typename Event, typename Handle>
    static auto createFromTag
      (Event const& event, Handle&& mainHandle, art::InputTag auxInputTag)
      {
        return makeAssociatedDataFrom
          <main_element_t, aux_element_t, metadata_t, data_tag>
          (event, auxInputTag, mainHandle->size());
      }
    
  }; // struct AssociatedDataProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an associated data wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @see `withAssociated()`
   * This class is (indirectly) called when using `proxy::withAssociated()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the proxy to the data
   * association between the main data type and an auxiliary one.
   * The interface required by `withAssociated()` includes:
   * * a static `make()` method creating and returning the associated data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the associated proxy (coming from
   *   `withAssociated()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized (see `withAssociated()` for an example).
   * The default implementation just wraps a one-to-many
   * `art::Assns<Main, Aux>` data product fulfilling "one-to-many sequential
   * association" requirement (see the "Definitions" section in `ProxyBase.h`
   * documentation).
   * 
   * The last template argument is designed for specialization of associations
   * in the context of a specific proxy type.
   */
  template <
    typename Main, typename Aux, typename Metadata,
    typename CollProxy, typename Tag = Aux
    >
  class AssociatedDataProxyMaker
    : public AssociatedDataProxyMakerBase<Main, Aux, Metadata, Tag>
  {
    //
    // Note that this implementation is here only to document how to derive
    // a AssociatedDataProxyMaker (specialization) from
    // AssociatedDataProxyMakerBase. It's just mirroring the base class.
    //
    using base_t = AssociatedDataProxyMakerBase<Main, Aux, Metadata, Tag>;
    
      public:
    
    /// Type of the main datum ("left").
    using typename base_t::main_element_t;
    
    /// Type of the auxiliary associated datum ("right").
    using typename base_t::aux_element_t;
    
    /// Type of the associated metadata.
    using typename base_t::metadata_t;
    
    /// Type of associated data proxy being created.
    using typename base_t::aux_collection_proxy_t;
    
    /// Type of _art_ association being used as input.
    using typename base_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of data product handle
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @tparam Args optional single type (`art::InputTag` required)
     * @param event the event to read associations from
     * @param mainHandle handle of the main collection data product
     * @param margs an object describing the main data product
     * @param args input tag for associated data, if different from main
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * This implementation requires `margs` object to be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... Args>
    static auto make(
      Event const& event, Handle&& mainHandle, MainArgs const& margs,
      Args&&... args
      )
      {
        return base_t::make(
          event,
          std::forward<Handle>(mainHandle),
          margs,
          std::forward<Args>(args)...
          );
      }
    
  }; // struct AssociatedDataProxyMaker<>
  
  /// @}
  // end Associated data
  
  
  /**
   * @defgroup LArSoftProxiesParallelData Parallel data infrastructure
   * @ingroup LArSoftProxyCustom
   * @brief Infrastructure for support of parallel data structures.
   * 
   * This infrastructure provides support for merging to a proxy data products
   * fulfilling the
   * @ref LArSoftProxyDefinitionParallelData "parallel data product"
   * requirements.
   * 
   * The typical pattern is to merge a parallel data product with
   * `withParallelData()`:
   * 
   * 
   * @{
   */

  //----------------------------------------------------------------------------
  /**
   * @brief Wraps a collection into a parallel data collection object.
   * @tparam AuxColl type of parallel data data product container
   * @tparam Aux type of parallel data to be associated to the main objects
   *             (if omitted: `AuxColl::value_type`)
   * @tparam Tag the tag labelling this associated data (if omitted: as `Aux`)
   * @param data data collection to be wrapped
   * @return a new `ParallelData` wrapping the information in `data`
   * 
   * The data collection must be non-temporary and it is treated as fulfilling
   * @ref LArSoftProxyDefinitionParallelData "parallel data product"
   * requirements.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * std::vector<recob::TrackFitHitInfo> trackData;
   * // ...
   * auto auxData = makeParallelDataFrom(trackData);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * where the tag of the parallel data is now `recob::TrackFitHitInfo` and
   * `auxData` behaviour becomes undefined as soon as `trackData` falls out of
   * scope.
   */
  template <
    typename AuxColl,
    typename Aux = util::collection_value_t<AuxColl>,
    typename Tag = Aux
    >
  auto makeParallelDataFrom(AuxColl const& data)
    { return proxy::makeParallelData<AuxColl, Aux, Tag>(data); }
  
  
  /**
   * @brief Creates and returns a parallel data collection object.
   * @tparam AuxColl type of parallel data data product container
   * @tparam Aux type of parallel data to be associated to the main objects
   *             (if omitted: `AuxColl::value_type`)
   * @tparam Tag the tag labelling this associated data (if omitted: as `Aux`)
   * @tparam Event type of event to read the data product from
   * @param event event to read the data product from
   * @param tag input tag of the parallel data product
   * @return a new `ParallelData` filled with data from `tag`
   * 
   * The data product being retrieved must fulfill the requirements of
   * @ref LArSoftProxyDefinitionParallelData "parallel data product".
   * 
   * At least one template type must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto auxData
   *   = makeParallelDataFrom<std::vector<recob::TrackFitHitInfo>>(event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this case, the `Aux` type is defined as `recob::TrackFitHitInfo`, as is
   * the tag.
   */
  template <typename AuxColl, typename Aux, typename Tag, typename Event>
  auto makeParallelDataFrom(Event const& event, art::InputTag tag);
  
  template <typename AuxColl, typename Aux, typename Event>
  auto makeParallelDataFrom(Event const& event, art::InputTag tag)
    { return makeParallelDataFrom<AuxColl, Aux, Aux, Event>(event, tag); }
  
  template <typename AuxColl, typename Event>
  auto makeParallelDataFrom(Event const& event, art::InputTag tag)
    {
      return makeParallelDataFrom<AuxColl, util::collection_value_t<AuxColl>, Event>
        (event, tag);
    }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates an parallel data wrapper for the specified types.
   * @tparam Main type of main datum (element)
   * @tparam AuxColl type of the parallel data collection
   * @tparam Aux type of the parallel data element
   * @tparam AuxTag tag labelling the parallel data collection
   * 
   * Usually, `AuxTag` is also the type of datum (element) in the parallel
   * collection.
   * 
   * This class works as a base class for `ParallelDataProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template <
    typename Main,
    typename AuxColl,
    typename Aux,
    typename AuxTag = util::collection_value_t<AuxColl>
    >
  struct ParallelDataProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum.
    using main_element_t = Main;
    
    /// Type of the auxiliary data product.
    using aux_collection_t = AuxColl;
    
    /// Type of the auxiliary datum.
    using aux_element_t = Aux;
    
    /// Type of associated data proxy being created.
    using aux_collection_proxy_t
       = details::ParallelData<aux_collection_t, aux_element_t, data_tag>;
    
    /**
     * @brief Create a parallel data proxy collection using main collection tag.
     * @tparam Event type of the event to read data from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs _(unused)_ any type convertible to `art::InputTag`
     * @param event the event to read data from
     * @param mainHandle _(unused)_ handle to the main collection data product
     * @param mainArgs an object describing the main data product
     * @return a parallel data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The `mainArgs` object is of an arbitrary type that must be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the parallel data collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
      {
        return createFromTag
          (event, std::forward<Handle>(mainHandle), art::InputTag(mainArgs));
      }
    
    /**
     * @brief Create a parallel data proxy collection using the specified tag.
     * @tparam Event type of the event to read data from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs (_unused_) any type convertible to `art::InputTag`
     * @param event the event to read associations from
     * @param mainHandle (_unused_) handle to the main collection data product
     * @param auxInputTag the tag of the data to be read
     * @return a parallel data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make(
      Event const& event, Handle&& mainHandle,
      MainArgs const&, art::InputTag auxInputTag
      )
      {
        return
          createFromTag(event, std::forward<Handle>(mainHandle), auxInputTag);
      }
    
    /**
     * @brief Create a parallel data proxy collection using the specified tag.
     * @tparam Event (_unused_) type of the event to read data from
     * @tparam Handle (_unused_) type of handle to the main data product
     * @tparam MainArgs (_unused_) any type convertible to `art::InputTag`
     * @param auxColl the collection to be wrapped
     * @return a parallel data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const&, Handle&&, MainArgs const&, aux_collection_t const& auxColl)
      {
        return
          makeParallelDataFrom<aux_collection_t, aux_element_t, data_tag>(auxColl);
      }
    
    
      private:
    template<typename Event, typename Handle>
    static auto createFromTag
      (Event const& event, Handle&&, art::InputTag auxInputTag)
      {
        return makeParallelDataFrom<aux_collection_t, aux_element_t, data_tag>
          (event, auxInputTag);
      }
    
  }; // struct ParallelDataProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an associated data wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @tparam AuxColl type of the auxiliary data (default: `std::vector<Aux>`)
   * @see `withParallelDataAs()`, `wrapParallelDataAs()`
   * 
   * This class is (indirectly) called when using `proxy::withParallelData()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the proxy to the auxiliary data
   * parallel to the main data type.
   * The interface required by `withParallelData()` includes:
   * * a static `make()` method creating and returning the auxiliary data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the associated proxy (coming from
   *   `withParallelData()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized (see `withParallelData()` for an example).
   * The default implementation just wraps a one-to-many
   * `std::vector<Aux>` data product fulfilling "parallel data product"
   * requirement (see the "Definitions" section in `ProxyBase.h` documentation).
   * 
   * The last template argument is designed for specialization of auxiliary data
   * in the context of a specific proxy type.
   */
  template<
    typename Main, typename Aux, typename CollProxy, typename Tag = Aux,
    typename AuxColl = std::vector<Aux>
    >
  class ParallelDataProxyMaker
    : public ParallelDataProxyMakerBase<Main, AuxColl, Aux, Tag>
  {
    //
    // Note that this implementation is here only to document how to derive
    // a ParallelDataProxyMaker (specialization) from
    // ParallelDataProxyMakerBase. It's just mirroring the base class.
    //
    using base_t = ParallelDataProxyMakerBase<Main, AuxColl, Aux, Tag>;
    
      public:
    
    /// Type of the main datum.
    using typename base_t::main_element_t;
    
    /// Type of the auxiliary data product.
    using typename base_t::aux_collection_t;
    
    /// Type of the auxiliary datum.
    using typename base_t::aux_element_t;
    
    /// Type of collection data proxy being created.
    using typename base_t::aux_collection_proxy_t;
    
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of data product handle
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @tparam Args optional single type (`art::InputTag` required)
     * @param event the event to read associations from
     * @param mainHandle handle of the main collection data product
     * @param margs an object describing the main data product
     * @param args input tag for parallel data, if different from main
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * This implementation requires `margs` object to be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... Args>
    static auto make(
      Event const& event, Handle&& mainHandle, MainArgs const& margs,
      Args&&... args
      )
      {
        return base_t::make(
          event,
          std::forward<Handle>(mainHandle),
          margs,
          std::forward<Args>(args)...
          );
      }
    
  }; // struct ParallelDataProxyMaker<>
  
  
  
  /// @}
  // end Parallel data
  
  
  /**
   * @defgroup LArSoftProxiesAuxProxy Infrastructure for proxies as auxiliary
   *           data.
   * @ingroup LArSoftProxyCustom
   * @brief Infrastructure to use a collection proxy as auxiliary data for
   *        another proxy.
   * @bug Broken in many ways. Do not use.
   * 
   * @{
   */
  
  //----------------------------------------------------------------------------
  template <
    typename Tag /* = Aux */,
    typename Aux /* = util::collection_value_t<AuxProxyColl>*/,
    typename AuxProxyColl
    >
  auto makeProxyAsParallelData(AuxProxyColl&& auxProxy)
    {
      return details::ProxyAsParallelData<AuxProxyColl, Aux, Tag>
        (std::move(auxProxy));
    } // makeProxyAsParallelData()
  
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates a proxy wrapper for merging into another proxy ("main").
   * @tparam Main type of main datum (element) of the main proxy
   * @tparam AuxProxy type ("proxy name") of the proxy being wrapped
   * @tparam AuxTag tag of the auxiliary proxy in the context of the main one
   * 
   * By default, `AuxTag` is the same as the proxy name.
   * 
   * This class works as a base class for `ProxyAsAuxProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template <
    typename Main,
    typename AuxProxy,
    typename AuxTag = AuxProxy
    >
  struct ProxyAsAuxProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum.
    using main_element_t = Main;
    
    /// Tag-type of the auxiliary proxy (not the type of the proxy!).
    using aux_proxy_t = AuxProxy;
    
    /**
     * @brief Create a parallel data proxy collection using the specified tag.
     * @tparam Event type of the event to read data from
     * @tparam Handle (_unused_) type of handle to the main data product
     * @tparam MainArgs (_unused_) any type convertible to `art::InputTag`
     * @tparam AuxArgs type of arguments for the creation of the auxiliary proxy
     * @param event event to create the proxy from
     * @param auxProxyTag tag for the creation of the auxiliary collection proxy
     * @param args other arguments for the creation of the auxiliary proxy
     * @return a auxiliary proxy data object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The tag of the main collection proxy is ignored even if present, and
     * the caller must specify it.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... AuxArgs>
    static auto make(
      Event const& event, Handle&&, MainArgs const&, art::InputTag auxProxyTag,
      AuxArgs&&... args
      )
      {
        auto auxProxy = makeAuxiliaryProxy
          (event, auxProxyTag, std::forward<AuxArgs>(args)...);
        return makeProxyAsParallelData
          <data_tag, util::collection_value_t<decltype(auxProxy)>>
          (std::move(auxProxy));
      }
    
    
      private:
    
    /// Creates the proxy to be used as parallel data.
    template <typename Event, typename... AuxArgs>
    static auto makeAuxiliaryProxy(
      Event const& event,
      art::InputTag auxProxyTag,
      AuxArgs&&... args
    )
      { 
        return getCollection<aux_proxy_t>
          (event, auxProxyTag, std::forward<AuxArgs>(args)...);
      }
    
    
  }; // struct ProxyAsAuxProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an auxiliary proxy wrapper for the specified proxy.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam AuxProxy type of proxy collection to be associated
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @see `withCollectionProxy()`
   * 
   * This class is (indirectly) called when using `proxy::withCollectionProxy()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the collection proxy that is used
   * as auxiliary data for the main data type.
   * The interface required by `withCollectionProxy()` includes:
   * * a static `make()` method creating and returning the auxiliary data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the auxiliary collection proxy
   *   (coming from `withCollectionProxy()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized.
   * The default implementation just uses `getCollection()` to create the
   * auxiliary proxy, and merges it to the main collection proxy in a fashion
   * similar to parallel data.
   * 
   * The template argument `CollProxy` is designed for specialization of
   * auxiliary data in the context of a specific proxy type.
   */
  template <
    typename Main,
    typename AuxProxy,
    typename CollProxy,
    typename Tag = util::collection_value_t<AuxProxy>
    >
  class ProxyAsAuxProxyMaker
    : public ProxyAsAuxProxyMakerBase<Main, AuxProxy, Tag>
    {};
  
  
  /// @}
  // end Proxy as auxiliary data
  
  
  // --- BEGIN Collection proxy infrastructure ---------------------------------
  /// @ingroup LArSoftProxyCollections
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
  
  
  //----------------------------------------------------------------------------
  /// The same as `withZeroOrOneMeta()`, but it also specified a tag.
  template <typename Aux, typename Metadata, typename AuxTag, typename... Args>
  auto withZeroOrOneMetaAs(Args&&... args) {
    using ArgTuple_t = std::tuple<Args&&...>;
    ArgTuple_t argsTuple(std::forward<Args>(args)...);
    return
      details::WithOneTo01AssociatedStruct<Aux, Metadata, ArgTuple_t, AuxTag>
      (std::move(argsTuple));
  } // withZeroOrOneAs()
  
  
  /// The same as `withZeroOrOne()`, but it also specified a tag for the data.
  template <typename Aux, typename AuxTag, typename... Args>
  auto withZeroOrOneAs(Args&&... args)
    {
      return
        withZeroOrOneMetaAs<Aux, void, AuxTag>(std::forward<Args>(args)...);
    }
  
  /**
   * @brief Helper function to merge one-to-(zero-or-one) associated data.
   * @tparam Aux type of associated data requested
   * @tparam Metadata type of metadata coming with the associated data
   * @tparam Args types of constructor arguments for associated data collection
   * @param args constructor arguments for the associated data collection
   * @return a temporary object that `getCollection()` knows to handle
   * @see withZeroOrOneMetaAs(), withZeroOrOne()
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for the delivered collection proxy to carry auxiliary data from an
   * association fulfilling the
   * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association"
   * requirements.
   * 
   * This data will be tagged with the type `Aux`. To use a different type as
   * tag, use `withZeroOrOneAs()` instead, specifying the tag as second
   * template argument, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct QuestionableVertex {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withZeroOrOneMeta<recob::Vertex, void>(defaultVertexTag),
   *   withZeroOrOneMetaAs<recob::Vertex, void, QuestionableVertex>
   *     (stinkyVertexTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * and, since we are not requesting any metadata, this is equivalent to
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct QuestionableVertex {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withZeroOrOne<recob::Vertex>(defaultVertexTag),
   *   withZeroOrOneAs<recob::Vertex, QuestionableVertex>(stinkyVertexTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The first vertex association (`"defaultVertexTag"`) will be accessed by
   * using the type `recob::Vertex` as tag, while the second one will be
   * accessed by the `QuestionableVertex` tag (which is better not be defined
   * in a local scope):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * for (auto&& track: tracks) {
   *   decltype(auto) vertex = track.get<recob::Vertex>();
   *   decltype(auto) maybeVertex = track.get<QuestionableVertex>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * 
   * Customization of the association proxy
   * =======================================
   * 
   * See the technical details about `withAssociated()`, which hold for this
   * function and related classes too.
   * 
   * 
   * Technical details
   * ==================
   * 
   * See the technical details about `withAssociated()`, which hold for this
   * function and related classes too.
   */
  template <typename Aux, typename Metadata, typename... Args>
  auto withZeroOrOneMeta(Args&&... args)
    {
      return
        withZeroOrOneMetaAs<Aux, Metadata, Aux>(std::forward<Args>(args)...); 
    }
  
  /// Works like `withZeroOrOneMeta()`, but for associations with no metadata.
  /// @see withZeroOrOneAs(), withZeroOrOneMeta()
  template <typename Aux, typename... Args>
  auto withZeroOrOne(Args&&... args)
    { return withZeroOrOneMeta<Aux, void>(std::forward<Args>(args)...); }
  
  
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
  
  
  //----------------------------------------------------------------------------
  /// The same as `withCollectionProxy()`, but it also specified a tag.
  /// @bug Broken in many ways. Do not use.
  template <typename AuxProxy, typename AuxTag, typename... Args>
  auto withCollectionProxyAs(Args&&... args)
    {
      using ArgTuple_t = std::tuple<Args&&...>;
      static_assert(
        std::is_convertible
          <std::decay_t<std::tuple_element_t<0U, ArgTuple_t>>, art::InputTag>(),
        "The first argument of withCollectionProxyAs() must be art::InputTag."
        );
      ArgTuple_t argsTuple(std::forward<Args>(args)...);
      return details::WithProxyAsAuxStructBase<AuxProxy, ArgTuple_t, AuxTag>
        (std::move(argsTuple));
    } // withCollectionProxyAs()
  
  //----------------------------------------------------------------------------
  /**
   * @brief Helper function to merge an auxiliary proxy into the proxy.
   * @tparam AuxProxy type (proxy tag) of auxiliary collection proxy requested
   * @tparam Args types of constructor arguments for parallel data proxy
   * @param args constructor arguments for the parallel data collection proxy
   * @return a temporary object that `getCollection()` knows to handle
   * @bug Broken in many ways. Do not use.
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for merging a collection proxy to carry auxiliary data structured as
   * another collection proxy, parallel to the main collection.
   * The function also bridges the information required to create a proxy to
   * that auxiliary data.
   * 
   * This data will be tagged with the type `AuxProxy`. To use a different type
   * as tag, use `withCollectionProxyAs()` instead, specifying the tag as second
   * template argument.
   * 
   * 
   * Customization of the auxiliary collection proxy
   * ================================================
   * 
   * The customization of auxiliary collection proxy happens in a fashion
   * similar to the customization presented in `withParallelData()`.
   * The customization point here is `ProxyAsAuxProxyMaker`.
   */
  template <typename AuxProxy, typename... Args>
  auto withCollectionProxy(Args&&... args)
    {
      return
        withCollectionProxyAs<AuxProxy, AuxProxy>(std::forward<Args>(args)...);
    }
  
  
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
      (Event const& event, art::InputTag tag, WithArgs&&... withArgs)
      {
        auto mainHandle = event.template getValidHandle<main_collection_t>(tag);
        return makeCollectionProxy(
          *mainHandle,
          withArgs.template createAuxProxyMaker<main_collection_proxy_t>
            (event, mainHandle, tag)...
          );
      } // make()
    
      private:
    // helper function to avoid typing the exact types of auxiliary collections
    template <typename MainColl, typename... AuxColl>
    static auto makeCollectionProxy(MainColl const& main, AuxColl&&... aux)
      {
        return CollectionProxy<MainColl, AuxColl...>
          (main, std::forward<AuxColl>(aux)...);
      }
    
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
  
  
  //----------------------------------------------------------------------------
  namespace details {
    
    //--------------------------------------------------------------------------
    //---  general infrastructure
    //--------------------------------------------------------------------------
    /// Constant expressing whether `Iter` type implements random access.
    template <typename Iter>
    constexpr bool is_random_access_iterator_v = std::is_same<
      typename std::iterator_traits<Iter>::iterator_category,
      std::random_access_iterator_tag
      >::value;
    
    
    //--------------------------------------------------------------------------
    //--- stuff for auxiliary data
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    /**
     * @brief Helper to create associated data proxy.
     * @tparam Aux type of data associated to the main one
     * @tparam Metadata type of metadata of the association
     * @tparam ArgTuple type of arguments required for the creation of proxy
     * @tparam ProxyMaker template type of the proxy maker class
     * @tparam AuxTag tag for the associated data (default: as `Aux`)
     * 
     * This class stores user arguments for the construction of a proxy to
     * associated data of type `Aux`.
     * It can use that information plus some additional one to create the
     * associated data itself. This additional information is provided by
     * `getCollection()`.
     * 
     * The association will be identified by type `AuxTag`.
     * 
     * This is not a customization point: to have a custom associated data
     * produced, choose and then specialize the `ProxyMaker` class.
     */
    template <
      typename Aux,
      typename Metadata,
      typename ArgTuple,
      template <typename CollProxy> class ProxyMaker,
      typename AuxTag /* = Aux */
      >
    class WithAssociatedStructBase {
      
      /// Type of main data product element from a proxy of type `CollProxy`.
      template <typename CollProxy>
      using main_t = typename CollProxy::main_element_t;
      
      /// Type of associated data.
      using aux_t = Aux;
      
      /// Type of associated metadata.
      using metadata_t = Metadata;
      
      /// Tag for the associated data (same as the data type itself).
      using tag = AuxTag;
      
      /// Class to create the data proxy associated to a `CollProxy`.
      template <typename CollProxy>
      using proxy_maker_t = ProxyMaker<CollProxy>;
      
        public:
      
      /// Type of association proxy created for the specified `CollProxy`.
      template <typename CollProxy>
      using aux_collection_proxy_t
        = typename proxy_maker_t<CollProxy>::aux_collection_proxy_t;
      
      /// Constructor: steals the arguments, to be used by
      /// `createAuxProxyMaker()`.
      WithAssociatedStructBase(ArgTuple&& args): args(std::move(args)) {}
      
      /// Creates the associated data proxy by means of `ProxyMaker`.
      template
        <typename CollProxy, typename Event, typename Handle, typename MainArgs>
      auto createAuxProxyMaker
        (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
        { 
          return createAssnProxyMaker<CollProxy>(
            event, std::forward<Handle>(mainHandle), mainArgs,
            std::make_index_sequence<NArgs>()
            );
        } // construct()
      
      
        protected:
      
      ArgTuple args; ///< Argument construction storage as tuple.
      
      /// Number of arguments stored.
      static constexpr std::size_t NArgs = std::tuple_size<ArgTuple>();
      
      // this method allows unpacking the arguments from the tuple
      template<
        typename CollProxy, typename Event, typename Handle, typename MainArgs,
        std::size_t... I
        >
      auto createAssnProxyMaker(
        Event const& event, Handle&& mainHandle, MainArgs const& mainArgs,
        std::index_sequence<I...>
        )
        {
          return proxy_maker_t<CollProxy>::make(
            event, mainHandle, mainArgs,
            std::get<I>(std::forward<ArgTuple>(args))...
            );
        }
      
    }; // struct WithAssociatedStructBase
    
    
    
    //--------------------------------------------------------------------------
    //--- stuff for parallel data collection (a form of auxiliary data)
    //--------------------------------------------------------------------------
    template <typename Aux, typename AuxTag, typename AuxColl /* = void */>
    struct ParallelDataProxyMakerWrapper {
      template <typename CollProxy>
      using maker_t = ParallelDataProxyMaker
        <typename CollProxy::main_element_t, Aux, CollProxy, AuxTag, AuxColl>;
    }; // struct ParallelDataProxyMakerWrapper<Aux, AuxColl>
    
    template <typename Aux, typename AuxTag>
    struct ParallelDataProxyMakerWrapper<Aux, AuxTag, void> {
      template <typename CollProxy>
      using maker_t = ParallelDataProxyMaker
        <typename CollProxy::main_element_t, Aux, CollProxy, AuxTag>;
    }; // struct ParallelDataProxyMakerWrapper<Aux>
    
    
    
    
    
    //--------------------------------------------------------------------------
    //--- stuff for associated data (a form of auxiliary data)
    //--------------------------------------------------------------------------
    template <
      typename Aux, typename Metadata /* = void */,
      typename AuxTag /* = Aux */
      >
    struct AssociatedDataProxyMakerWrapper {
      template <typename CollProxy>
      using maker_t = AssociatedDataProxyMaker
        <typename CollProxy::main_element_t, Aux, Metadata, CollProxy, AuxTag>;
    };
    
    
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
    // the class is actually an alias whose definition was above
    
    
    //--------------------------------------------------------------------------
    //---  Stuff for one-to-(zero or one) associated data
    //--------------------------------------------------------------------------
    template <
       typename Aux, typename Metadata /* = void */,
       typename AuxTag /* = Aux */
       >
    struct OneTo01DataProxyMakerWrapper {
      template <typename CollProxy>
      using maker_t = OneTo01DataProxyMaker
        <typename CollProxy::main_element_t, Aux, Metadata, CollProxy, AuxTag>;
    };
    
    
    //--------------------------------------------------------------------------
    //---  Stuff for collection proxy as auxiliary data
    //--------------------------------------------------------------------------
    /**
     * @brief Object presenting a proxy as parallel data for another one.
     * @tparam AuxProxyColl type of the parallel data collection
     * @tparam Aux type of the associated object
     * @tparam Tag tag this data is labeled with
     * 
     * This object inherits its interface from `proxy::ParallelData`.
     * In addition, it owns the proxy it wraps.
     */
    template <
      typename AuxProxyColl,
      typename Aux /* = util::collection_value_t<AuxProxyColl> */,
      typename Tag /* = Aux */
      >
    struct ProxyAsParallelData
      : private AuxProxyColl
      , public ParallelData<AuxProxyColl, Aux, Tag>
    {
      /// Steals and wraps collection `proxy`.
      ProxyAsParallelData(AuxProxyColl&& proxy)
        : AuxProxyColl(std::move(proxy))
        , ParallelData<AuxProxyColl, Aux, Tag>
          (static_cast<AuxProxyColl const*>(this))
        {}
      
      // explicitly select the tag from the parallel data (same as Tag)
      using typename ParallelData<AuxProxyColl, Aux, Tag>::tag;
    }; // class ProxyAsParallelData<>
    
    
    //--------------------------------------------------------------------------
    /**
     * @brief Helper to create a proxy as auxiliary data for another proxy.
     * @tparam AuxProxy type of collection proxy associated to the main one
     * @tparam ArgTuple type of arguments required for the creation of proxy
     * @tparam AuxTag tag for the associated data (default: as `Aux`)
     * 
     * This class stores user arguments for the construction of a collection
     * proxy to be used as auxiliary data for another proxy.
     * It can use those arguments plus some additional one to create the
     * collection proxy data itself. This additional information is provided by
     * `getCollection()`.
     * 
     * The auxiliary data will be identified by type `AuxTag`.
     * 
     * This is not a customization point: to have a custom associated data
     * produced, choose and then specialize the `ProxyAsAuxProxyMaker` class.
     */
    template <
      typename AuxProxy,
      typename ArgTuple,
      typename AuxTag /* = AuxProxy */
      >
    class WithProxyAsAuxStructBase {
      
      static_assert(
        std::is_convertible
          <std::decay_t<std::tuple_element_t<0U, ArgTuple>>, art::InputTag>(),
        "The first argument of WithProxyAsAuxStructBase must be art::InputTag."
        );
      
      /// Type of main data product element from a proxy of type `CollProxy`.
      template <typename CollProxy>
      using main_t = typename CollProxy::main_element_t;
      
      /// Type of auxiliary proxy.
      using aux_proxy_t = AuxProxy;
      
      /// Tag for the associated data (same as the data type itself).
      using tag = AuxTag;
      
      /// Class to create the data proxy associated to a `CollProxy`.
      template <typename CollProxy>
      using proxy_maker_t
        = ProxyAsAuxProxyMaker<main_t<CollProxy>, aux_proxy_t, CollProxy, tag>;
      
        public:
      
      /// Constructor: steals the arguments, to be used by
      /// `createAuxProxyMaker()`.
      WithProxyAsAuxStructBase(ArgTuple&& args): args(std::move(args)) {}
      
      /// Creates the associated data proxy by means of `ProxyAsAuxProxyMaker`.
      template
        <typename CollProxy, typename Event, typename Handle, typename MainArgs>
      auto createAuxProxyMaker
        (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
        {
          return createAuxProxyImpl<CollProxy>(
            event, std::forward<Handle>(mainHandle), mainArgs,
            std::make_index_sequence<NArgs>()
            );
        } // construct()
      
      
        protected:
      
      ArgTuple args; ///< Argument construction storage as tuple.
      
      /// Number of arguments stored.
      static constexpr std::size_t NArgs = std::tuple_size<ArgTuple>();
      
      // this method allows unpacking the arguments from the tuple
      template<
        typename CollProxy, typename Event, typename Handle, typename MainArgs,
        std::size_t... I
        >
      auto createAuxProxyImpl(
        Event const& event, Handle&& mainHandle, MainArgs const& mainArgs,
        std::index_sequence<I...>
        )
        {
          return proxy_maker_t<CollProxy>::make(
            event, mainHandle, mainArgs,
            std::get<I>(std::forward<ArgTuple>(args))...
            );
        }
      
    }; // struct WithProxyAsAuxStructBase
    
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
} // namespace proxy


//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
namespace proxy {
  
  namespace details {
    
    //--------------------------------------------------------------------------
    template <typename Assns, typename = void>
    struct AssnWithMetadata: std::false_type {};
      
    template <typename Assns>
    struct AssnWithMetadata
      <Assns, std::enable_if_t<util::always_true_v<typename Assns::data_t>>>
      : std::true_type
    {};
    
    template <typename Assns>
    constexpr bool AssnWithMetadata_v = AssnWithMetadata<Assns>();
    
    
    //--------------------------------------------------------------------------
    template <typename Assns, typename /* = void */>
    struct AssnsMetadataTypeStruct {
      using type = void;
    }; // struct AssnsMetadataTypeStruct
    
    template <typename Assns>
    struct AssnsMetadataTypeStruct
      <Assns, std::enable_if_t<AssnWithMetadata_v<Assns>>>
    {
      using type = typename Assns::data_t;
    };
    
    
    //--------------------------------------------------------------------------
    template <typename Assns, typename /* = void */>
    struct AssnsIteratorTypeStruct {
      using type = typename Assns::assn_iterator;
    }; // struct AssnsIteratorTypeStruct
    
    
    template <typename Assns>
    struct AssnsIteratorTypeStruct
      <Assns, std::enable_if_t<AssnWithMetadata_v<Assns>>>
    {
      using type = typename Assns::const_iterator;
    };
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
  //----------------------------------------------------------------------------
  //--- makeAssociatedDataFrom() implementations
  //----------------------------------------------------------------------------
  template <
    typename Main, typename Aux, typename Metadata, typename Tag,
    typename Event
    >
  auto makeAssociatedDataFrom
    (Event const& event, art::InputTag tag, std::size_t minSize /* = 0 */)
  {
    using Main_t = Main;
    using Aux_t = Aux;
    using Metadata_t = Metadata;
    using AssociatedData_t
      = details::AssociatedData<Main_t, Aux_t, Metadata_t, Tag>;
    using Assns_t = typename AssociatedData_t::assns_t;
    
    return
      makeAssociatedDataFrom<Tag>(*(event.template getValidHandle<Assns_t>(tag)));
    
  } // makeAssociatedDataFrom(tag)
  
  
  //----------------------------------------------------------------------------
  template <
    typename Aux, typename Metadata, typename Tag,
    typename Handle, typename Event
    >
  auto makeAssociatedDataFrom
    (Handle&& handle, Event const& event, art::InputTag tag)
  {
    // Handle::value_type is the main data product type (a collection)
    using Main_t = util::collection_value_t<typename Handle::value_type>;
    using Aux_t = Aux;
    using Metadata_t = Metadata;
    return makeAssociatedDataFrom<Main_t, Aux_t, Metadata_t, Tag>
      (event, tag, handle->size());
  } // makeAssociatedDataFrom(handle)
  
  
  
  //----------------------------------------------------------------------------
  //--- makeOneTo01dataFrom() implementations
  //----------------------------------------------------------------------------
  template <
    typename Main, typename Aux, typename Metadata,
    typename Tag,
    typename Event
    >
  auto makeOneTo01dataFrom
    (Event const& event, art::InputTag tag, std::size_t minSize /* = 0 */)
  {
    using Main_t = Main;
    using Aux_t = Aux;
    using Metadata_t = Metadata;
    using AssociatedData_t
      = details::OneTo01Data<Main_t, Aux_t, Metadata_t, Tag>;
    using Assns_t = typename AssociatedData_t::assns_t;
    
    return makeOneTo01dataFrom<Tag>
      (*(event.template getValidHandle<Assns_t>(tag)), minSize);
    
  } // makeOneTo01dataFrom(tag)
  
  
  //----------------------------------------------------------------------------
  template <
    typename Aux, typename Metadata,
    typename Tag,
    typename Handle, typename Event
    >
  auto makeOneTo01dataFrom
    (Handle&& handle, Event const& event, art::InputTag tag)
  {
    // Handle::value_type is the main data product type (a collection)
    using Main_t = util::collection_value_t<typename Handle::value_type>;
    using Aux_t = Aux;
    using Metadata_t = Metadata;
    return makeOneTo01dataFrom<Main_t, Aux_t, Metadata_t, Tag>
      (event, tag, handle->size());
  } // makeOneTo01dataFrom(handle)
  
  
  
  //----------------------------------------------------------------------------
  //--- makeParallelDataFrom() implementations
  //----------------------------------------------------------------------------
  template <typename AuxColl, typename Aux, typename Tag, typename Event>
  auto makeParallelDataFrom(Event const& event, art::InputTag tag) {
    return makeParallelDataFrom<AuxColl, Aux, Tag>
      (*(event.template getValidHandle<AuxColl>(tag)));
  } // makeParallelDataFrom()
  
  
  //----------------------------------------------------------------------------
  //---  CollectionProxyMaker specializations
  //----------------------------------------------------------------------------
  // none so far
  
  //----------------------------------------------------------------------------
  
  
} // namespace proxy


//------------------------------------------------------------------------------
//--- implementation of specializations of std::get() for art::AssnsNode
//------------------------------------------------------------------------------
namespace proxy {
  namespace details {
    
    template <std::size_t I, typename L, typename R, typename D>
    struct AssnsNodeGetter; // incomplete type, except for specializations...
    
    
    template <typename L, typename R, typename D>
    struct AssnsNodeGetter<0U, L, R, D> {
      
      using AssnsNode_t = art::AssnsNode<L, R, D>;
      using Element_t = std::tuple_element_t<0U, AssnsNode_t>;
      
      static constexpr Element_t& get(AssnsNode_t& node) noexcept
        { return node.first; }
      
      static constexpr Element_t const& get(AssnsNode_t const& node) noexcept
        { return node.first; }
      
      static constexpr Element_t&& get(AssnsNode_t&& node) noexcept
        { return std::move(node.first); }
      
      static constexpr Element_t const&& get(AssnsNode_t const&& node) noexcept
        { return std::move(node.first); }
      
    }; // struct AssnsNodeGetter<0U>
    
    
    template <typename L, typename R, typename D>
    struct AssnsNodeGetter<1U, L, R, D> {
      
      using AssnsNode_t = art::AssnsNode<L, R, D>;
      using Element_t = std::tuple_element_t<1U, AssnsNode_t>;
      
      static constexpr Element_t& get(AssnsNode_t& node) noexcept
        { return node.second; }
      
      static constexpr Element_t const& get(AssnsNode_t const& node) noexcept
        { return node.second; }
      
      static constexpr Element_t&& get(AssnsNode_t&& node) noexcept
        { return std::move(node.second); }
      
      static constexpr Element_t const&& get(AssnsNode_t const&& node) noexcept
        { return std::move(node.second); }
      
    }; // struct AssnsNodeGetter<1U>
    
    template <typename L, typename R, typename D>
    struct AssnsNodeGetter<2U, L, R, D> {
      
      using AssnsNode_t = art::AssnsNode<L, R, D>;
      using Element_t = std::tuple_element_t<2U, AssnsNode_t>;
      
      static constexpr Element_t& get(AssnsNode_t& node) noexcept
        { return node.data; }
      
      static constexpr Element_t const& get(AssnsNode_t const& node) noexcept
        { return node.data; }
      
      static constexpr Element_t&& get(AssnsNode_t&& node) noexcept
        { return std::move(node.data); }
      
      static constexpr Element_t const&& get(AssnsNode_t const&& node) noexcept
        { return std::move(node.data); }
      
    }; // struct AssnsNodeGetter<2U>
    
    
  } // namespace details
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_PROXYBASE_H
