/**
 * @file   lardata/RecoBaseProxy/Track.cxx
 * @brief  Implementation file for `proxy::Track`.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * @see    Track.h
 * 
 */

// LArSoft libraries
#include "lardata/RecoBaseProxy/Track.h" // proxy namespace

#if 0
//------------------------------------------------------------------------------
//---  proxy::TrackPointIterator implementation
//------------------------------------------------------------------------------
proxy::TrackPointIterator proxy::details::TrackPointIteratorBox::begin() const
  { return TrackPointIterator(*track, 0U); }
proxy::TrackPointIterator proxy::details::TrackPointIteratorBox::end() const
  { return TrackPointIterator(*track, track->track().NPoints()); }

//------------------------------------------------------------------------------
//---  proxy::Tracks implementation
//------------------------------------------------------------------------------
proxy::Track proxy::Tracks::getProxyAt(std::size_t index) const {
  
  return { Base_t::getMainAt(index), hitsPerTrack[index] };
  
} // proxy::Tracks::getProxyAt()

//------------------------------------------------------------------------------

  template <typename CollProxy>
  struct CollectionProxyMaker {
    
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
     * @return a collection proxy to `recob::Track` collection at specified `tag`
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
          withArgs.template createAssnProxyMaker<main_collection_proxy_t>
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
    
  }; // struct CollectionProxyMaker<>

#endif // 0
