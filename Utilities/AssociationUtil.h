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

  // method to create a 1 to 1 association
  // indx is the location in the input std::vector<T> of the object you wish to 
  // associate with the art::Ptr<U>
  template<class T, class U> static bool CreateAssn(art::EDProducer const& prod,
						    art::Event            &evt, 
						    std::vector<T>        &a,
						    art::Ptr<U>            b,
						    art::Assns<U,T>       &assn,
						    size_t                indx=UINT_MAX);

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

}// end namespace

//----------------------------------------------------------------------
template<class T, class U> inline bool util::CreateAssn(art::EDProducer const& prod,
							art::Event            &evt, 
							std::vector<T>        &a,
							art::Ptr<U>            b,
							art::Assns<U,T>       &assn,
							size_t                 indx)
{
  bool ret = true;
  
  if(indx == UINT_MAX) indx = a.size()-1;
  
  try{
    art::ProductID aid = prod.getProductID< std::vector<T> >(evt);
    art::Ptr<T> aptr(aid, indx, evt.productGetter(aid));
    assn.addSingle(b, aptr);
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
template<class T, class U> inline bool util::CreateAssn(art::EDProducer const&     prod,
							art::Event                &evt, 
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
							std::vector<U>        &b,
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
						   std::vector<T>        &b,
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

#endif  //ASSOCIATIONUTIL_H
