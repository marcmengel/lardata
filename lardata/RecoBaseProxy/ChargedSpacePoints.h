/**
 * @file   lardata/RecoBaseProxy/ChargedSpacePoints.h
 * @brief  Offers `proxy::ChargedSpacePoints` and `proxy::SpacePointWithCharge`
 *         class for `recob::SpacePoint` with `recob:Charge` access.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 20, 2017
 * 
 */

/**
 * @brief Proxy for a `recob::SpacePoint` collection with associated charge.
 * @ingroup LArSoftProxyReco
 * 
 * Some algorithms can reconstruct the position of some activity in the active
 * volume of the detector, locating it as a 3D point, and also estimate the
 * electric charge associated to that localized activity.
 * 
 * The prescription is for such algorithms to produce as output two data
 * products:
 * 1. a collection of points (`std::vector<recob::SpacePoint>`) containing the
 *    location of each reconstructed activity
 * 2. a collection of charge information (`std::vector<recob::PointCharge>`)
 *    containing the reconstructed charge for each activity.
 * 
 * The two data products are _implicitly_ associated by counting the same number
 * of elements, and being sorted so that the _i_-th charge pertains the _i_-th
 * location (@ref LArSoftProxyDefinitionParallelData "parallel data product"
 * requirement).
 * 
 * Access to this information is facilitated via the low-overhead data proxy
 * `proxy::ChargedSpacePoints`.
 * 
 * 
 * 
 * Obtaining a charged space point proxy
 * ======================================
 * 
 * The charged space point proxy can be obtained directly as:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto spacePoints = proxy::getChargedSpacePoints(event, pointsTag);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * where `tag` is the input tag for both the space points and the charge, which
 * must have been created by the same module (usually, the tag is just the
 * module label).
 * 
 * 
 * Extending the proxy collection object
 * --------------------------------------
 * 
 * This proxy can be augmented with the usual proxy operations (see
 * `proxy::getCollection()`); `proxy::getChargedSpacePoints()` calls
 * are in fact equivalent to:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * proxy::getCollection<proxy::ChargedSpacePoints>
 *   (event, tag, proxy::withParallelData<recob::PointCharge>(), ...);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 * Types of proxies, and what to do with them
 * ===========================================
 * 
 * Currently there are two different type of proxy-like objects for space points
 * with added charge.
 * Each one supports a specific concept:
 * 
 * * `proxy::ChargedSpacePoints` represents the whole collection of space
 *   points; it covers the location and the associated charge. It is obtained
 *   by calling `getChargedSpacePoints()` as described above.
 * * `proxy::SpacePointWithCharge` represents a single point with charge
 *   information; the location and the charge can be accessed through it.
 *   These proxies are obtained from the space point collection proxy above.
 * 
 * For the details of the interface and the information that is exposed by each
 * of these proxy classes, please refer to each class documentation. In
 * particular, see `proxy::ChargedSpacePoints` documentation for more usage
 * examples.
 * 
 */

#ifndef LARDATA_RECOBASEPROXY_CHARGEDSPACEPOINTS_H
#define LARDATA_RECOBASEPROXY_CHARGEDSPACEPOINTS_H


// LArSoft libraries
#include "lardata/RecoBaseProxy/ProxyBase.h" // proxy namespace
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/PointCharge.h"
#include "larcorealg/Geometry/geo_vectors_utils.h" // geo::vect namespace

// framework libraries


namespace proxy {
  
  //----------------------------------------------------------------------------
  
  /**
   * @brief Proxy tag for a `recob::SpacePoint` collection with charge.
   * @see `proxy::SpacePointWithCharge`, `proxy::getChargedSpacePoints()`
   * @ingroup LArSoftProxyReco
   *
   * This type can be used to get a proxy for `recob::SpacePoint` collection
   * with charge. Normally, you want to use
   * `proxy::getChargedSpacePoints()` directly instead.
   */
  /*
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto spacePoints = proxy::getChargedSpacePoints(event, pointsTag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * An example of usage for a simple space point processing loop:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * void MyAnalyzer::analyze(art::Event const& event) {
   *   
   *   auto points = proxy::getChargedSpacePoints(event, pointsTag);
   *   
   *   if (points.empty()) {
   *     mf::LogVerbatim("ProxyTest")
   *       << "No points in '" << pointsTag.encode() << "'";
   *     return;
   *   }
   *   
   *   mf::LogVerbatim log("ProxyTest");
   *   for (auto point: points) {
   *     log << "\nPoint at " << point.position() << " (ID=" << point.ID()
   *       << ") has ";
   *     if (point.hasCharge()) log << "charge " << point.charge();
   *     else                   log << "no charge";
   *   } // for point
   *   
   *   mf::LogVerbatim("ProxyTest") << "Collection '" << pointsTag.encode()
   *     << "' contains " << points.size() << " points.";
   *   
   * } // MyAnalyzer::analyze()
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this example, the charged space point proxy accesses the information
   * exclusively via its specific interface.
   * 
   * Unfortunately, the proxy object (`point` in the example) can be of a
   * different class depending on which data is merged into it (via optional
   * arguments after the `tag` argument).
   * This implies than when passing proxies as arguments to functions, template
   * types must be used. For example, the following code is equivalent to the
   * one above, but with methods processing a single point (a point proxy):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * template <typename Point>
   * void MyAnalyzer::processPoint(Point const& point) const {
   *   
   *   mf::LogVerbatim log("ProxyTest");
   *   log << "\nPoint at " << point.position() << " (ID=" << point.ID()
   *     << ") has ";
   *   if (point.hasCharge()) log << "charge " << point.charge();
   *   else                   log << "no charge";
   *   
   * } // MyAnalyzer::processPoint()
   * 
   * 
   * void MyAnalyzer::proxyUsageExample(art::Event const& event) {
   *   
   *   auto points = proxy::getChargedSpacePoints(event, pointsTag);
   *   
   *   if (points.empty()) {
   *     mf::LogVerbatim("ProxyTest")
   *       << "No points in '" << pointsTag.encode() << "'";
   *     return;
   *   }
   *   
   *   mf::LogVerbatim("ProxyTest") << "Collection '" << pointsTag.encode()
   *     << "' contains " << points.size() << " points.";
   *   
   *   for (auto point: points) {
   *     
   *     processPoint(point);
   *     
   *   } // for point
   *   
   * } // MyAnalyzer::proxyUsageExample()
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * A new, filtered collection of proxies can be created with obvious means and
   * with a less-than-friendly declaration:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * std::vector<decltype(points)::element_proxy_t> strongPoints;
   * for (auto point: points) {
   *   if (point.charge() >= 30.0) strongPoints.push_back(point);
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The collection thus created (`strongPoints`) is valid also after the
   * collection proxy (`points`) has fallen out of scope.
   * 
   * @note `proxy::ChargedSpacePoints` is *not* the type of the collection proxy
   *       returned by `proxy::getChargedSpacePoints()`.
   */
  struct ChargedSpacePoints {
    
    /// Type of the main collection.
    using SpacePointDataProduct_t = std::vector<recob::SpacePoint>;
    
    /// Tag used for the "standard" charge information.
    using ChargeTag = recob::PointCharge;
    
  }; // struct ChargedSpacePoints
  
  
  
  /// Define the traits of `proxy::ChargedSpacePoints` proxy.
  template <>
  struct CollectionProxyMakerTraits<ChargedSpacePoints>
    : public
      CollectionProxyMakerTraits<ChargedSpacePoints::SpacePointDataProduct_t>
  {};
  
  
  //--------------------------------------------------------------------------
  /**
    * @brief Proxy class for charged space point proxy elements.
    * @tparam CollProxy type of point proxy collection to get data from
    * @ingroup LArSoftProxyReco
    *
    * For details on the space point interface see `proxy::ChargedSpacePoints`.
    */
  template <typename CollProxy>
  struct SpacePointWithCharge: public CollectionProxyElement<CollProxy> {
    
    using base_t = CollectionProxyElement<CollProxy>; ///< Base type.
    using base_t::base_t; // inherit constructors
    
      public:
    
    // --- BEGIN data object access --------------------------------------------
    /// @{
    /// @name Full data object access
        
    /// Returns the original space point.
    recob::SpacePoint const& point() const { return base_t::operator*(); }
    
    /// @}
    // --- END data object access ----------------------------------------------
    
    /// Returns the `recob::PointCharge` object with the complete charge
    /// information.
    recob::PointCharge const& chargeInfo() const
      { return base_t::template get<ChargedSpacePoints::ChargeTag>(); }
    
    // --- BEGIN space point access --------------------------------------------
    /// @{
    /// @name Direct space point interface
    
    /// Returns the ID of the space point.
    auto ID() const { return point().ID(); }
    
    /// Returns the position of the space point.
    geo::Point_t position() const
      { return geo::vect::makePointFromCoords(point().XYZ()); }
    
    /// @}
    // --- END space point access ----------------------------------------------
    
    
    // --- BEGIN charge access -------------------------------------------------
    /// @{
    /// @name Direct charge interface
    
    /// Returns the charge associated to this point
    /// @see `recob::PointCharge::charge()`
    recob::PointCharge::Charge_t charge() const
      { return chargeInfo().charge(); }
    
    /// Returns whether the charge associated to the space point is valid.
    /// @see recob::PointCharge::hasCharge()
    bool hasCharge() const { return chargeInfo().hasCharge(); }
    
    /// @}
    // --- END charge access ---------------------------------------------------
    
    
  }; // SpacePointWithCharge<>
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Proxy collection class for space points associated to charge.
   * @tparam MainColl type of space point collection
   * @tparam AuxColl types of auxiliary data collections
   * @see `proxy::CollectionProxyBase`
   * 
   * This proxy collection allows access to space point and charge collection
   * directly:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto points = proxy::getChargedSpacePoints(event, pointsTag);
   * auto const& spacePoints = points.spacePoints();
   * auto const& charges = points.charges();
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * For the rest of the specific interface, see `proxy::SpacePointWithCharge`.
   */
  template <typename MainColl, typename... AuxColl>
  class ChargedSpacePointsCollectionProxy
    : public CollectionProxyBase<SpacePointWithCharge, MainColl, AuxColl...>
  {
    using base_t
      = CollectionProxyBase<SpacePointWithCharge, MainColl, AuxColl...>;
    using base_t::base_t;
    
      public:
    
    /// Returns the original collection of space points.
    auto const& spacePoints() const
      { return base_t::main(); }
    
    /// Returns the original collection of charge information.
    auto const& charges() const
      {
        return base_t::template get<ChargedSpacePoints::ChargeTag>().dataRef();
      }
    
  }; // ChargedSpacePointsCollectionProxy
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Adds additional `recob::PointCharge` information to the proxy.
   * @param inputTag the data product label to read the data from
   * @return an object driving `getCollection()` to use a `recob::PointCharge`
   * @ingroup LArSoftProxyReco
   * 
   * The `proxy::ChargedSpacePoints` returned by
   * `proxy::getChargedSpacePoints()` comes with its charge by default.
   * If a different one is required, `proxy::withCharge()` can be used:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto points = proxy::getCollection<proxy::ChargedSpacePoints>
   *   (event, pointsTag, proxy::withCharge(calibrationTag));
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The charge from the data product of type `std::vector<recob::PointCharge>`
   * with input tag `calibrationTag` will be used as default charge of the proxy
   * instead of the one from `pointsTag`.
   */
  template <typename Tag = proxy::ChargedSpacePoints::ChargeTag>
  auto withCharge(art::InputTag inputTag)
    { return proxy::withParallelDataAs<recob::PointCharge, Tag>(inputTag); }
  
  
  /**
   * @brief Creates and returns a proxy to space points with associated charge.
   * @tparam Event type of the event to read data from
   * @tparam Args additional arguments
   * @param event event to read the data from
   * @param inputTag tag for the reconstructed space points and charge data
   * @param withArgs additional elements to be merged into the proxy
   * @return a proxy to space points with associated charge
   * @see `proxy::ChargedSpacePoints`, `proxy::SpacePointWithCharge`,
   *      `proxy::getCollection()`
   * 
   * This function initializes and return a space point proxy with associated
   * charge.
   * The proxy has a `recob::SpacePoint` as main data product, and it comes with
   * an association of a single `recob::PointCharge` per space point, that can
   * be accessed with thag `proxy::ChargedSpacePoints::ChargeTag` (although the
   * specific interface documented in `proxy::ChargedSpacePoints` and
   * `proxy::SpacePointWithCharge` is more convenient).
   * 
   * Additional elements can be merged into the proxy, in the usual way of
   * `proxy::getCollection()`.
   * 
   */
  template <typename Event, typename... Args>
  auto getChargedSpacePoints
    (Event const& event, art::InputTag inputTag, Args&&... withArgs)
    {
      return proxy::getCollection<ChargedSpacePoints>(
        event, inputTag, withCharge(inputTag), std::forward<Args>(withArgs)...
        );
    } // getChargedSpacePoints()
  
  
  //----------------------------------------------------------------------------
  /// Specialization to create a proxy for `recob::ChargedSpacePoints`
  /// collection.
  /// 
  /// The specialization uses the customized proxy element
  /// `proxy::SpacePointWithCharge`.
  template <>
  struct CollectionProxyMaker<ChargedSpacePoints>
    : public CollectionProxyMakerBase<ChargedSpacePoints>
  {
    
    /// Traits of the collection proxy for the collection proxy maker.
    using maker_base_t = CollectionProxyMakerBase<ChargedSpacePoints>;
    
    /// Type of main collection proxy.
    using typename maker_base_t::main_collection_proxy_t;
    
    /// Type of element of the main collection.
    using typename maker_base_t::main_collection_t;
    
    /**
     * @brief Creates and returns a collection proxy for `recob::SpacePoint`
     *        based on `proxy::ChargedSpacePoints` tag and with the requested
     *        associated data.
     * @tparam Event type of the event to read the information from
     * @tparam WithArgs type of arguments for associated data
     * @param event event to read the information from
     * @param tag input tag of the `recob::SpacePoint` collection data product
     * @param withArgs optional associated objects to be included
     * @return a collection proxy to `recob::SpacePoint` collection with `tag`
     * 
     * For each argument in `withArgs`, an action is taken. Usually that is to
     * add an association to the proxy.
     */
    template <typename Event, typename... WithArgs>
    static auto make
      (Event const& event, art::InputTag tag, WithArgs&&... withArgs)
      {
        auto mainHandle = event.template getValidHandle<main_collection_t>(tag);
        auto proxy = makeCollectionProxy(
          *mainHandle,
          withArgs.template createAuxProxyMaker<main_collection_proxy_t>
            (event, mainHandle, tag)...
          );
        return proxy;
      } // make()
    
      private:
    template <typename MainColl, typename... AuxColl>
    using coll_proxy_t
      = ChargedSpacePointsCollectionProxy<MainColl, AuxColl...>;
    
    // helper function to avoid typing the exact types of auxiliary collections
    template <typename MainColl, typename... AuxColl>
    static auto makeCollectionProxy(MainColl const& main, AuxColl&&... aux)
      {
        return coll_proxy_t<MainColl, AuxColl...>
          (main, std::forward<AuxColl>(aux)...);
      }
    
  }; // struct CollectionProxyMaker<ChargedSpacePoints>
  
  
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_CHARGEDSPACEPOINTS_H
