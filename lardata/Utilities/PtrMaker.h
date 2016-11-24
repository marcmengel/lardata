/**
 *  @file   lardata/lardata/Utilities/PtrMaker.h
 *
 *  @brief  Helper function to create an art::Ptr
 *
 */

#include "art/Framework/Core/EDProducer.h"
//#include "art/Framework/Core/FindManyP.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "art/Framework/Core/ModuleMacros.h"
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

