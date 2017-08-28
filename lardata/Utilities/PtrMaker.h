////////////////////////////////////////////////////////////////////////
// Class:       PtrMaker
// File:        lardata/lardata/Utilities/PtrMaker.h
// 
// Author:      Saba Sehrish
//
// NOTE: PtrMaker is now in art/Persistency/Common/PtrMaker.h.
//       This header is now a wrapper and is retained for backwards compatibility.
//
// Description: A common pattern is to create a collection A, 
// and a collection B, create art::Ptrs to the objects in each of 
// the collection and then create associations between the objects 
// in the two collections. The purpose of art::PtrMaker is to simplify 
// the process of creating art::Assns by providing a utility to create 
// art::Ptrs. It is a two step process to create an art::Ptr with this 
// approach. 
// Step I has two cases; the product and ptrs are constructed 
// in the same module, or in different modules. For each case, there 
// is a way to construct a PtrMaker object. 
//
// case 1: Construct a PtrMaker object that creates Ptrs in to a collection  
// of type C, the most common one is std::vector and hence is the default, 
// created by the module of type MODULETYPE, where the collection has 
// instance name "instance", which is optional. For example, to create 
// a PtrMaker for an std::vector of A in an event evt, and current module, 
// we will use the PtrMaker as follows:
// PtrMaker<A>make_Aptr(evt, *this);
// 
// case 2: In this case, the collection of type C is created in another 
// module. We need the product ID to create an object of PtrMaker. The
// way to get a product ID is to first get an art::Handle and then use 
// "id()". Assuming, h is the art::Handle to the data product, and evt is 
// art::Event, then we will use it as follows: 
// art::Handle<std::vector<A>> h;
// PtrMaker<A> make_Aptr(evt, h.id());
//
// Step II: Use an index to create an art::Ptr to an object in the 
// slot indicated by "index"
// auto const a = make_Aptr(index);
// 
////////////////////////////////////////////////////////////////////////

// PtrMaker is now in art
#include "art/Persistency/Common/PtrMaker.h"

