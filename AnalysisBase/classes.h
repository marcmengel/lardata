// Build a dictionary.
//
// $Id: classes.h,v 1.8 2010/04/12 18:12:28  Exp $
// $Author:  $
// $Date: 2010/04/12 18:12:28 $
// 
// Original author Rob Kutschke, modified by klg
//
// Notes:
// 1) The system is not able to deal with
//    art::Wrapper<std::vector<std::string> >;
//    The problem is somewhere inside root's reflex mechanism
//    and Philippe Canal says that it is ( as of March 2010) a
//    known problem.  He also says that they do not have any
//    plans to fix it soon.  We can always work around it 
//    by putting the string inside another object.

#include "art/Persistency/Common/PtrVector.h" 
#include "art/Persistency/Common/Wrapper.h"
#include "art/Persistency/Common/Assns.h"

#include "AnalysisBase/Calorimetry.h"
#include "AnalysisBase/ParticleID.h"
#include "AnalysisBase/FlashMatch.h"
#include "AnalysisBase/CosmicTag.h"
#include "AnalysisBase/T0.h"

#include "RecoBase/OpFlash.h"
#include "RecoBase/PFParticle.h"
#include "RecoBase/Track.h"
#include "RecoBase/PCAxis.h"
#include "RecoBase/Hit.h"
#include "RecoBase/Shower.h"
#include "RecoBase/Cluster.h"

#include "RawData/ExternalTrigger.h"


// make dummy variables of the PtrVector types so that we are
// sure to generate the dictionaries for them
namespace {
  art::PtrVector<recob::PFParticle> pfpv;
  art::PtrVector<recob::Track>      tpv;
  art::PtrVector<recob::PCAxis>     pcpv;
  art::PtrVector<recob::OpFlash>    flv;
  art::PtrVector<anab::Calorimetry> cpv;
  art::PtrVector<anab::ParticleID>  ppv;
  art::PtrVector<anab::FlashMatch>  fmv;
}

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

template class std::vector<anab::Calorimetry>;
template class std::vector<anab::ParticleID>;
template class std::vector<anab::FlashMatch>;
template class std::vector<anab::CosmicTag>;
template class std::vector<anab::T0>;

template class art::Ptr<anab::Calorimetry>;
template class art::Ptr<anab::ParticleID>;
template class art::Ptr<anab::FlashMatch>;
template class art::Ptr<anab::CosmicTag>;
template class art::Ptr<anab::T0>;

template class std::pair< art::Ptr<anab::Calorimetry>, art::Ptr<recob::Track>        >;
template class std::pair< art::Ptr<recob::Track>,      art::Ptr<anab::Calorimetry>   >;
template class std::pair< art::Ptr<anab::Calorimetry>, art::Ptr<recob::Shower>       >;
template class std::pair< art::Ptr<recob::Shower>,     art::Ptr<anab::Calorimetry>   >;
template class std::pair< art::Ptr<anab::ParticleID>,  art::Ptr<recob::Track>        >;
template class std::pair< art::Ptr<recob::Track>,      art::Ptr<anab::ParticleID>    >;
template class std::pair< art::Ptr<anab::FlashMatch>,  art::Ptr<recob::OpFlash>      >;
template class std::pair< art::Ptr<recob::OpFlash>,    art::Ptr<anab::FlashMatch>    >;
template class std::pair< art::Ptr<anab::FlashMatch>,  art::Ptr<recob::Track>        >;
template class std::pair< art::Ptr<recob::Track>,      art::Ptr<anab::FlashMatch>    >;
template class std::pair< art::Ptr<anab::T0>,          art::Ptr<recob::Track>        >;
template class std::pair< art::Ptr<recob::Track>,      art::Ptr<anab::T0>            >;
template class std::pair< art::Ptr<anab::T0>,          art::Ptr<recob::Shower>       >;
template class std::pair< art::Ptr<recob::Shower>,     art::Ptr<anab::T0>            >;
template class std::pair< art::Ptr<anab::T0>,          art::Ptr<raw::ExternalTrigger> >;
template class std::pair< art::Ptr<raw::ExternalTrigger>, art::Ptr<anab::T0>          >;
template class std::pair< art::Ptr<recob::OpFlash>,    art::Ptr<anab::T0>            >;
template class std::pair< art::Ptr<anab::T0>,          art::Ptr<recob::OpFlash>      >;

template class std::pair< art::Ptr<anab::FlashMatch>,  art::Ptr<recob::Cluster>      >;
template class std::pair< art::Ptr<recob::Cluster>,    art::Ptr<anab::FlashMatch>    >;
template class std::pair< art::Ptr<anab::FlashMatch>,  art::Ptr<anab::CosmicTag>    >;
template class std::pair< art::Ptr<anab::CosmicTag>,  art::Ptr<anab::FlashMatch>    >;

template class std::pair< art::Ptr<recob::PFParticle>, art::Ptr<anab::CosmicTag>    >;
template class std::pair< art::Ptr<recob::Track>,      art::Ptr<anab::CosmicTag>    >;
template class std::pair< art::Ptr<recob::PCAxis>,     art::Ptr<anab::CosmicTag>    >;
template class std::pair< art::Ptr<recob::Cluster>,    art::Ptr<anab::CosmicTag>    >;
template class std::pair< art::Ptr<recob::Hit>,        art::Ptr<anab::CosmicTag>    >;
template class std::pair< art::Ptr<anab::CosmicTag>,   art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<anab::CosmicTag>,   art::Ptr<recob::Track>      >;
template class std::pair< art::Ptr<anab::CosmicTag>,   art::Ptr<recob::PCAxis>     >;
template class std::pair< art::Ptr<anab::CosmicTag>,   art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<anab::CosmicTag>,   art::Ptr<recob::Hit>        >;

template class art::Assns<anab::Calorimetry, recob::Track,     	void>;
template class art::Assns<recob::Track,      anab::Calorimetry, void>;
template class art::Assns<anab::Calorimetry, recob::Shower,    	void>;
template class art::Assns<recob::Shower,     anab::Calorimetry, void>;
template class art::Assns<anab::ParticleID,  recob::Track,     	void>;
template class art::Assns<recob::Track,      anab::ParticleID,  void>;
template class art::Assns<recob::Track,      anab::FlashMatch,  void>;
template class art::Assns<recob::OpFlash,    anab::FlashMatch,  void>;
template class art::Assns<anab::FlashMatch,  recob::Track,      void>;
template class art::Assns<anab::FlashMatch,  recob::OpFlash,    void>;
template class art::Assns<recob::Track,      anab::CosmicTag,   void>;
template class art::Assns<recob::Cluster,    anab::CosmicTag,   void>;
template class art::Assns<recob::Hit,        anab::CosmicTag,   void>;
template class art::Assns<recob::PFParticle, anab::CosmicTag,   void>;
template class art::Assns<anab::CosmicTag,   recob::PFParticle, void>;
template class art::Assns<anab::CosmicTag,   recob::Track,      void>;
template class art::Assns<anab::CosmicTag,   recob::PCAxis,     void>;
template class art::Assns<anab::CosmicTag,   recob::Cluster,    void>;
template class art::Assns<anab::CosmicTag,   recob::Hit,        void>;
template class art::Assns<anab::T0,          recob::Track,      void>;
template class art::Assns<anab::T0,          recob::Shower,     void>;
template class art::Assns<anab::T0,          raw::ExternalTrigger, void>;
template class art::Assns<anab::T0,          recob::OpFlash,    void>;
template class art::Assns<recob::Track,      anab::T0,          void>;
template class art::Assns<recob::Shower,     anab::T0,          void>;
template class art::Assns<raw::ExternalTrigger, anab::T0,       void>;
template class art::Assns<recob::OpFlash,    anab::T0,          void>;


template class art::Assns<anab::CosmicTag,   anab::FlashMatch,  void>;
template class art::Assns<anab::FlashMatch,  anab::CosmicTag,   void>;
template class art::Assns<recob::Cluster,    anab::FlashMatch,  void>;
template class art::Assns<anab::FlashMatch,  recob::Cluster,    void>;

template class art::Wrapper< art::Assns<anab::Calorimetry, recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::Track,      anab::Calorimetry, void> >;
template class art::Wrapper< art::Assns<anab::Calorimetry, recob::Shower,     void> >;
template class art::Wrapper< art::Assns<recob::Shower,     anab::Calorimetry, void> >;
template class art::Wrapper< art::Assns<anab::ParticleID,  recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::Track,      anab::ParticleID,  void> >;
template class art::Wrapper< art::Assns<recob::Track,      anab::FlashMatch,  void> >;
template class art::Wrapper< art::Assns<recob::OpFlash,    anab::FlashMatch,  void> >;
template class art::Wrapper< art::Assns<anab::FlashMatch,  recob::Track,      void> >;
template class art::Wrapper< art::Assns<anab::FlashMatch,  recob::OpFlash,    void> >;
template class art::Wrapper< art::Assns<anab::CosmicTag,   recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<anab::CosmicTag,   recob::Track,      void> >;
template class art::Wrapper< art::Assns<anab::CosmicTag,   recob::PCAxis,     void> >;
template class art::Wrapper< art::Assns<anab::CosmicTag,   recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<anab::CosmicTag,   recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, anab::CosmicTag,   void> >;
template class art::Wrapper< art::Assns<recob::Track,      anab::CosmicTag,   void> >;
template class art::Wrapper< art::Assns<recob::PCAxis,     anab::CosmicTag,   void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    anab::CosmicTag,   void> >;
template class art::Wrapper< art::Assns<recob::Hit,        anab::CosmicTag,   void> >;

template class art::Wrapper< art::Assns<anab::FlashMatch,  anab::CosmicTag,   void> >;
template class art::Wrapper< art::Assns<anab::CosmicTag,   anab::FlashMatch,  void> >;
template class art::Wrapper< art::Assns<anab::FlashMatch , recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    anab::FlashMatch,  void> >;

template class art::Wrapper< art::Assns<anab::T0,          recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::Track,      anab::T0,          void> >;
template class art::Wrapper< art::Assns<anab::T0,          recob::Shower,     void> >;
template class art::Wrapper< art::Assns<recob::Shower,     anab::T0,          void> >;
template class art::Wrapper< art::Assns<anab::T0,          raw::ExternalTrigger, void> >;
template class art::Wrapper< art::Assns<raw::ExternalTrigger, anab::T0,       void> >;
template class art::Wrapper< art::Assns<anab::T0,          recob::OpFlash,    void> >;
template class art::Wrapper< art::Assns<recob::OpFlash,    anab::T0,          void> >;

template class art::Wrapper< std::vector<anab::Calorimetry>    >;
template class art::Wrapper< std::vector<anab::ParticleID>     >;
template class art::Wrapper< std::vector<anab::FlashMatch>     >;
template class art::Wrapper< std::vector<anab::CosmicTag>      >;
template class art::Wrapper< std::vector<anab::T0>             >;
