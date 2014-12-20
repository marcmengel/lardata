////////////////////////////////////////////////////////////////////////////
// \version $ $
//
// \brief Utility object to perform functions of association 
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#ifndef ASSOCIATIONUTIL_H
#define ASSOCIATIONUTIL_H

#include "art/Persistency/Common/Ptr.h"
#include "art/Persistency/Common/PtrVector.h"
#include "art/Framework/Principal/Event.h"
#include "art/Persistency/Common/Assns.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/FindMany.h" 
#include "art/Framework/Core/FindManyP.h" 
#include "art/Framework/Core/FindOneP.h" 
#include "art/Framework/Core/FindOne.h" 
#include "art/Persistency/Provenance/ProductID.h" 
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace util {

  // see https://cdcvs.fnal.gov/redmine/projects/art/wiki/Inter-Product_References
  // for information about using art::Assns

  /**
   * @brief Creates a single one-to-one association
   * @tparam T type of the new object to associate
   * @tparam U type of the object already in the data product or art::Ptr
   * @param prod reference to the producer that will write the vector a
   * @param evt reference to the current event
   * @param a vector of data products that are in, or will be put into, evt
   * @param b art::Ptr to the (new) object to be associated to the one in a
   * @param assn reference to association object where the new one will be put
   * @param a_instance name of the instance that will be used for a in evt
   * @param indx index of the element in a to be associated with b (default: the last element)
   * @return whether the operation was successful (can it ever fail??)
   *
   * As example of usage: create a wire/raw digit association.
   * This code should live in the art::EDProduce::produce() method.
   * The raw::RawDigit product was created already by a DigitModuleLabel module.
   * The code is supposed to produce one recob::Wire for each existing
   * raw::RawDigit, and contextually associate the new wire to the source digit.
   * We are also assuming that there might be different RawDigit sets produced
   * by the same producer: we identify the one we care of by the string
   * spill_name and we create wires and associations with the same label
   * for convenience.
   *     
   *     // this is the original list of digits, thawed from the event
   *     art::Handle< std::vector<raw::RawDigit>> digitVecHandle;
   *     evt.getByLabel(DigitModuleLabel, spill_name, digitVecHandle);
   *     
   *     // the collection of wires that will be written as data product
   *     std::unique_ptr<std::vector<recob::Wire>> wirecol(new std::vector<recob::Wire>);
   *     // ... and an association set
   *     std::unique_ptr<art::Assns<raw::RawDigit,recob::Wire>> WireDigitAssn
   *       (new art::Assns<raw::RawDigit,recob::Wire>);
   *     
   *     for(size_t iDigit = 0; iDigit < digitVecHandle->size(); ++iDigit) {
   *       // turn the digit into a art::Ptr:
   *       art::Ptr<raw::RawDigit> digit_ptr(digitVecHandle, iDigit);
   *       
   *       // store the wire in its final position in the data product;
   *       // the new wire is currently the last of the list
   *       wirecol->push_back(std::move(wire));
   *       
   *       // add an association between the last object in wirecol
   *       // (that we just inserted) and digit_ptr
   *       if (!util::CreateAssn(*this, evt, *wirecol, digit_ptr, *WireDigitAssn, spill_name)) {
   *         throw art::Exception(art::errors::InsertFailure)
   *           << "Can't associate wire #" << (wirecol->size() - 1)
   *           << " with raw digit #" << digit_ptr.key();
   *       } // if failed to add association
   *     
   *     } // for digits
   *     
   *     evt.put(std::move(wirecol), spill_name);
   *     evt.put(std::move(WireDigitAssn), spill_name);
   *     
   */
  template<class T, class U>
  bool CreateAssn(art::EDProducer const& prod,
                  art::Event            &evt,
                  std::vector<T>        &a,
                  art::Ptr<U>            b,
                  art::Assns<U,T>       &assn,
                  std::string           a_instance,
                  size_t                indx=UINT_MAX
                  );

  // method to create a 1 to 1 association
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the art::Ptr<U>
  template<class T, class U> 
  inline bool CreateAssn(art::EDProducer const& prod,
						    art::Event            &evt, 
						    std::vector<T>        &a,
						    art::Ptr<U>            b,
						    art::Assns<U,T>       &assn,
						    size_t                indx=UINT_MAX)
    { return CreateAssn(prod, evt, a, b, assn, std::string(), indx); }
	

  // method to create a 1 to 1 association
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the art::Ptr<U>
  template<class T, class U> static bool CreateAssn(art::EDProducer const& prod,
						    art::Event            &evt, 
						    art::Ptr<T>           &a,
						    art::Ptr<U>            b,
						    art::Assns<U,T>       &assn);
  
  // method to create a 1 to many association, with the many being of type U
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the art::PtrVector<U>
  template<class T, class U> static bool CreateAssn(art::EDProducer const& prod,
						    art::Event            &evt, 
						    std::vector<T>        &a,
						    art::PtrVector<U>      b,
						    art::Assns<T,U>       &assn,
						    size_t                 indx=UINT_MAX);

  // method to create a 1 to many association, with the many being of type U
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the art::PtrVector<U>
  template<class T, class U> static bool CreateAssn(art::EDProducer const&     prod,
						    art::Event                &evt, 
						    art::Ptr<T>               &a,
						    std::vector< art::Ptr<U> > b,
						    art::Assns<T,U>           &assn);

  // method to create a 1 to many association, with the many being of type U
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the art::PtrVector<U>
  template<class T, class U> static bool CreateAssn(art::EDProducer const&     prod,
						    art::Event                &evt, 
						    std::vector<T>            &a,
						    std::vector< art::Ptr<U> > b,
						    art::Assns<T,U>           &assn,
						    size_t                     indx=UINT_MAX);

  // method to create a 1 to many association, with the many being of type U
  // in this case the U objects are not yet stored in the event and are in a 
  // std::vector collection instead, so the method gets the product id for those
  // as well as for the 1.  Also specify the range of entries to use from the
  // std::vector collection of U objects
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the objects in the std::vector<U>
  template<class T, class U> static bool CreateAssn(art::EDProducer const& prod,
						    art::Event            &evt, 
						    std::vector<T>        &a,
						    std::vector<U>        &b,
						    art::Assns<T,U>       &assn,
						    size_t                 startU,
						    size_t                 endU,
						    size_t                 indx=UINT_MAX);

  // method to create a 1 to many association, with the many being of type T
  // in this case the T objects are not yet stored in the event and are in a 
  // std::vector collection instead, so the method gets the product id for those
  // as well as for the 1.  Also specify the range of entries to use from the
  // std::vector collection of T objects
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the objects in the std::vector<T>
  template<class T> static bool CreateSameAssn(art::EDProducer const& prod,
					       art::Event            &evt, 
					       std::vector<T>        &a,
					       std::vector<T>        &b,
					       art::Assns<T,T>       &assn,
					       size_t                 startU,
					       size_t                 endU,
					       size_t                 indx=UINT_MAX);

  // method to return all objects of type U that are not associated to 
  // objects of type T. Label is the module label that would have produced
  // the associations and likely the objects of type T
  // this method assumes there is a one to many relationship between T and U
  // for example if you want to get all recob::Hits
  // that are not associated to recob::Clusters
  // std::vector<const recob::Hit*> hits = FindUNotAssociatedToU<recob::Cluster>(art::Handle<recob::Hit>, ...);
  template<class T, class U> static std::vector<const U*> FindUNotAssociatedToT(art::Handle<U>     b, 
										art::Event  const& evt,
										std::string const& label);

  // method to return all objects of type U that are not associated to 
  // objects of type T. Label is the module label that would have produced
  // the associations and likely the objects of type T
  // this method assumes there is a one to many relationship between T and U
  // for example if you want to get all recob::Hits
  // that are not associated to recob::Clusters
  // std::vector< art::Ptr<recob::Hit> > hits = FindUNotAssociatedToTP<recob::Cluster>(art::Handle<recob::Hit>, ...);
  template<class T, class U> static std::vector< art::Ptr<U> > FindUNotAssociatedToTP(art::Handle<U>     b, 
										      art::Event  const& evt,
										      std::string const& label);

  // Methods make getting simple ART-independent association information.
  // --- GetAssociatedVectorOneI takes in a handle to an association, and a handle to a product on the event.
  //     The ouput is a vector of with the same number of entries as the handle to the product, containing an index
  //     to the location of one associated product in that product's collection.
  // --- GetAssociatedVectorOneP takes in a handle to an association, and a handle to a product on the event.
  //     The ouput is a vector of with the same number of entries as the handle to the product, containing a pointer
  //     to one associated product.
  // --- GetAssociatedVectorManyI takes in a handle to an association, and a handle to a product on the event.
  //     The ouput is a vector of with the same number of entries as the handle to the product, containing a vector 
  //     of indices that give the locations of all associated products in those products' collection.
  // --- GetAssociatedVectorManyP takes in a handle to an association, and a handle to a product on the event.
  //     The ouput is a vector of with the same number of entries as the handle to the product, containing a vector 
  //     of pointers to all associated products.

  template<class T,class U> static std::vector<size_t> GetAssociatedVectorOneI(art::Handle< art::Assns<T,U> > h,
									      art::Handle< std::vector<T> > index_p);
  template<class T,class U> static std::vector<const U*> GetAssociatedVectorOneP(art::Handle< art::Assns<T,U> > h,
										 art::Handle< std::vector<T> > index_p);

  template<class T,class U> static std::vector< std::vector<size_t> > GetAssociatedVectorManyI(art::Handle< art::Assns<T,U> > h,
											       art::Handle< std::vector<T> > index_p);
  template<class T,class U> static std::vector< std::vector<const U*> > GetAssociatedVectorManyP(art::Handle< art::Assns<T,U> > h,
												 art::Handle< std::vector<T> > index_p);


}// end namespace

//----------------------------------------------------------------------
template<class T, class U>
inline bool util::CreateAssn(
  art::EDProducer const& prod,
  art::Event            &evt,
  std::vector<T>        &a,
  art::Ptr<U>            b,
  art::Assns<U,T>       &assn,
  std::string            a_instance,
  size_t                 indx /* = UINT_MAX */
  )
{
  if (indx == UINT_MAX) indx = a.size()-1;
  
  try{
    art::ProductID aid = prod.getProductID< std::vector<T>>(evt, a_instance);
    art::Ptr<T> aptr(aid, indx, evt.productGetter(aid));
    assn.addSingle(b, aptr);
    return true;
  }
  catch(cet::exception &e){
    mf::LogWarning("AssociationUtil")
      << "unable to create requested art:Assns, exception thrown: " << e;
    return false;
  }
  
}


//----------------------------------------------------------------------
template<class T, class U> inline bool util::CreateAssn(art::EDProducer const& /*prod*/,
							art::Event            &/*evt*/, 
							art::Ptr<T>           &a,
							art::Ptr<U>            b,
							art::Assns<U,T>       &assn)
{
  bool ret = true;
  
  try{
    assn.addSingle(b, a);
  }
  catch(cet::exception &e){
    mf::LogWarning("AssociationUtil") << "unable to create requested "
				      << "art:Assns, exception thrown: "
				      << e;
    ret = false;
  }
  
  return ret;
}


//----------------------------------------------------------------------
template<class T, class U> inline bool util::CreateAssn(art::EDProducer const& prod,
							art::Event            &evt, 
							std::vector<T>        &a,
							art::PtrVector<U>      b,
							art::Assns<T,U>       &assn,
							size_t                 indx)
{
  
  bool ret = true;
  
  if(indx == UINT_MAX) indx = a.size()-1;
  
  try{
    art::ProductID aid = prod.getProductID< std::vector<T> >(evt);
    art::Ptr<T> aptr(aid, indx, evt.productGetter(aid));
    for(size_t i = 0; i < b.size(); ++i) assn.addSingle(aptr, b[i]);
  }
  catch(cet::exception &e){
    mf::LogWarning("AssociationUtil") << "unable to create requested "
				      << "art:Assns, exception thrown: "
				      << e;
    ret = false;
  }
  
  return ret;
}

//----------------------------------------------------------------------
template<class T, class U> inline bool util::CreateAssn(art::EDProducer const&     prod,
							art::Event                &evt, 
							std::vector<T>            &a,
							std::vector< art::Ptr<U> > b,
							art::Assns<T,U>           &assn,
							size_t                     indx)
{
  
  bool ret = true;
  
  if(indx == UINT_MAX) indx = a.size()-1;
  
  try{
    art::ProductID aid = prod.getProductID< std::vector<T> >(evt);
    art::Ptr<T> aptr(aid, indx, evt.productGetter(aid));
    for(size_t i = 0; i < b.size(); ++i) assn.addSingle(aptr, b[i]);
  }
  catch(cet::exception &e){
    mf::LogWarning("AssociationUtil") << "unable to create requested "
				      << "art:Assns, exception thrown: "
				      << e;
    ret = false;
  }
  
  return ret;
}

//----------------------------------------------------------------------
template<class T, class U> inline bool util::CreateAssn(art::EDProducer const&     /*prod*/,
							art::Event                &/*evt*/, 
							art::Ptr<T>               &a,
							std::vector< art::Ptr<U> > b,
							art::Assns<T,U>           &assn)
{
  
  bool ret = true;
  
  try{
    for(size_t i = 0; i < b.size(); ++i) assn.addSingle(a, b[i]);
  }
  catch(cet::exception &e){
    mf::LogWarning("AssociationUtil") << "unable to create requested "
				      << "art:Assns, exception thrown: "
				      << e;
    ret = false;
  }
  
  return ret;
}

//----------------------------------------------------------------------
template<class T, class U> inline bool util::CreateAssn(art::EDProducer const& prod,
							art::Event            &evt, 
							std::vector<T>        &a,
							std::vector<U>        &/*b*/,
							art::Assns<T,U>       &assn,
							size_t                 startU,
							size_t                 endU,
							size_t                 indx=UINT_MAX)
{
  
  bool ret = true;
  
  if(indx == UINT_MAX) indx = a.size()-1;
  
  try{
    art::ProductID aid = prod.getProductID< std::vector<T> >(evt);
    art::Ptr<T> aptr(aid, indx, evt.productGetter(aid));
    for(size_t i = startU; i < endU; ++i){
      art::ProductID bid = prod.getProductID< std::vector<U> >(evt);
      art::Ptr<U> bptr(bid, i, evt.productGetter(bid));
      assn.addSingle(aptr, bptr);
    }
  }
  catch(cet::exception &e){
    mf::LogWarning("AssociationUtil") << "unable to create requested "
				      << "art:Assns, exception thrown: "
				      << e;
    ret = false;
  }
  
  return ret;
}

//----------------------------------------------------------------------
template<class T> inline bool util::CreateSameAssn(art::EDProducer const& prod,
						   art::Event            &evt, 
						   std::vector<T>        &a,
						   std::vector<T>        &/*b*/,
						   art::Assns<T,T>       &assn,
						   size_t                 startU,
						   size_t                 endU,
						   size_t                 indx=UINT_MAX)
{
  
  bool ret = true;
  
  if(indx == UINT_MAX) indx = a.size()-1;
  
  try{
    art::ProductID aid = prod.getProductID< std::vector<T> >(evt);
    art::Ptr<T> aptr(aid, indx, evt.productGetter(aid));
    for(size_t i = startU; i < endU; ++i){
      art::ProductID bid = prod.getProductID< std::vector<T> >(evt);
      art::Ptr<T> bptr(bid, i, evt.productGetter(bid));

      // have to do the addSingle twice, once for each direction
      // as we are associating the same types
      assn.addSingle(aptr, bptr);
      assn.addSingle(bptr, aptr);
    }
  }
  catch(cet::exception &e){
    mf::LogWarning("AssociationUtil") << "unable to create requested "
				      << "art:Assns, exception thrown: "
				      << e;
    ret = false;
  }
  
  return ret;
}

//----------------------------------------------------------------------
template<class T, class U> inline std::vector<const U*> util::FindUNotAssociatedToT(art::Handle<U>     b,
										    art::Event  const& evt,
										    std::string const& label)
{

  // Do a FindOne for type T for each object of type U
  // If the FindOne returns an invalid maybe ref, add the pointer
  // of object type U to the return vector

  std::vector<const U*> notAssociated;

  art::FindOne<T> fa(b, evt, label);

  for(size_t u = 0; u < b->size(); ++u){
    cet::maybe_ref<T const> t(fa.at(u));
    if( !t.isValid() ){
      art::Ptr<U> ptr(b, u);
      notAssociated.push_back(ptr.get());
    }
  }
//   
  return notAssociated;
}

//----------------------------------------------------------------------
template<class T, class U> inline std::vector< art::Ptr<U> > util::FindUNotAssociatedToTP(art::Handle<U>     b,
											  art::Event  const& evt,
											  std::string const& label)
{

  // Do a FindOneP for type T for each object of type U
  // If the FindOne returns an invalid maybe ref, add the pointer
  // of object type U to the return vector

  std::vector< art::Ptr<U> > notAssociated;

  art::FindOneP<T> fa(b, evt, label);

  for(size_t u = 0; u < b->size(); ++u){
    cet::maybe_ref<T const> t(fa.at(u));
    if( !t.isValid() ){
      art::Ptr<U> ptr(b, u);
      notAssociated.push_back(ptr);
    }
  }
  
  return notAssociated;
}



template<class T,class U> inline std::vector<size_t> util::GetAssociatedVectorOneI(art::Handle< art::Assns<T,U> > h,
										   art::Handle< std::vector<T> > index_p)
{
  std::vector<size_t> associated_index(index_p->size());
  for(auto const& pair : *h)
    associated_index.at(pair.first.key()) = pair.second.key();
  return associated_index;
}

template<class T,class U> inline std::vector<const U*> util::GetAssociatedVectorOneP(art::Handle< art::Assns<T,U> > h,
										     art::Handle< std::vector<T> > index_p)
{
  std::vector<const U*> associated_pointer(index_p->size());
  for(auto const& pair : *h)
    associated_pointer.at(pair.first.key()) = &(*(pair.second));
  return associated_pointer;
}

template<class T,class U> inline std::vector< std::vector<size_t> > util::GetAssociatedVectorManyI(art::Handle< art::Assns<T,U> > h,
												   art::Handle< std::vector<T> > index_p)
{
  std::vector< std::vector<size_t> > associated_indices(index_p->size());
  for(auto const& pair : *h)
    associated_indices.at(pair.first.key()).push_back(pair.second.key());
  return associated_indices;
}

template<class T,class U> inline std::vector< std::vector<const U*> > util::GetAssociatedVectorManyP(art::Handle< art::Assns<T,U> > h,
												     art::Handle< std::vector<T> > index_p)
{
  std::vector< std::vector<const U*> > associated_pointers(index_p->size());
  for(auto const& pair : *h)
    associated_pointers.at(pair.first.key()).push_back( &(*(pair.second)) );
  return associated_pointers;
}


#endif  //ASSOCIATIONUTIL_H
