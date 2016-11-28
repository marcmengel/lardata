/**
 * @file   lardata/lardata/Utilities/PtrMaker.h
 *
 * @brief  Helper function to create an art::Ptr
 * 
 * Description This utility simplifies creation of an art::Ptr 
 * It is a two step process to create an art::Ptr. 
 * 1. Construct a PtrMaker object that creates Ptrs in to a collection  
 *    of type C created by the module of type MODULETYPE, where the 
 *    collection has instance name "instance". These collections
 *    can be from a different module or the same module. 
 *    Constructors: 
 *    PtrMaker<A>(Event, ProductID, instance); 
 *    PtrMaker<A>(Event, MODULETYPE, instance);
 * 2. Use an index to create an art::Ptr to an object in the 
 *    slot indicated by "index"
 *    
 *    
 * Example: 
 * lar::PtrMaker<recob::SpacePoint> make_spptr(evt, *this);
 * auto const spptr = make_spptr(index);
 */

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"


#include <iostream>

namespace lar {
   // to create art::Ptrs in to a particular collection in an event
   template <class T>
   class PtrMaker {
   public:
      //Creates a PtrMaker that creates Ptrs in to a collection of type C created by the module of type MODULETYPE, where the collection has instance name "instance"
      template <class MODULETYPE, class C = std::vector<T>>
      PtrMaker(art::Event const & evt, MODULETYPE const& module, std::string const& instance = std::string());
      
      //use this constructor when making Ptrs to products created in other modules
      PtrMaker(art::Event const & evt, const art::ProductID & prodId, std::string const& instance = std::string());
      
      //Creates a Ptr to an object in the slot indicated by "index"
      art::Ptr<T> operator()(std::size_t index) const;
      
   private:
      const art::ProductID prodId;
      art::EDProductGetter const* prodGetter;
   };
   
   template <class T>
   template <class MODULETYPE, class C>
   PtrMaker<T>::PtrMaker(art::Event const & evt, MODULETYPE const& module, std::string const & instance)
   : prodId(module.template getProductID<C>(evt, instance))
   , prodGetter(evt.productGetter(prodId))
   {  }
   
   template <class T>
   PtrMaker<T>::PtrMaker(art::Event const & evt, const art::ProductID & pid, std::string const & instance)
   : prodId(pid)
   , prodGetter(evt.productGetter(pid))
   {  }
   
   template <class T>
   art::Ptr<T> PtrMaker<T>::operator()(size_t index) const
   {
      art::Ptr<T> artPtr(prodId, index, prodGetter);
      return artPtr;
   }
}
// end of namespace

