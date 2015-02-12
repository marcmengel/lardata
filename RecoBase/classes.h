/**
 * @file classes.h
 * @brief Dictionary selection for RecoBase
 * 
 * Original author Rob Kutschke, modified by klg
 * 
 * @note The system is not able to deal with
 * `art::Wrapper<std::vector<std::string>>`.
 * The problem is somewhere inside ROOT's REFLEX mechanism
 * and Philippe Canal says that it is (as of March 2010) a known problem.
 * He also says that they do not have any plans to fix it soon.
 * We can always work around it by putting the string inside another object.
 */

#include "art/Persistency/Common/PtrVector.h" 
#include "art/Persistency/Common/Wrapper.h"
#include "art/Persistency/Common/Assns.h"

#include "SimpleTypesAndConstants/geo_types.h"
#include "RawData/RawDigit.h"
#include "RecoBase/Cluster.h"
#include "RecoBase/Hit.h"
#include "RecoBase/Shower.h"
#include "RecoBase/Track.h"
#include "RecoBase/Seed.h"
#include "RecoBase/EndPoint2D.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/Vertex.h"
#include "RecoBase/Event.h"
#include "RecoBase/OpHit.h"
#include "RecoBase/OpFlash.h"
#include "RecoBase/Wire.h"
#include "RecoBase/PFParticle.h"
#include "RecoBase/PCAxis.h"

// make dummy variables of the PtrVector types so that we are
// sure to generate the dictionaries for them
namespace {
  art::PtrVector<recob::Hit>        hitpv;
  art::PtrVector<recob::Event>      epv;
  art::PtrVector<recob::Cluster>    cpv;
  art::PtrVector<recob::Seed>       sepv;
  art::PtrVector<recob::Shower>     spv;
  art::PtrVector<recob::SpacePoint> sppv;
  art::PtrVector<recob::Track>      tpv;
  art::PtrVector<recob::Vertex>     vpv;
  art::PtrVector<recob::OpHit>      opv;
  art::PtrVector<recob::OpFlash>    fpv;
  art::PtrVector<recob::Wire>       wpv;
  art::PtrVector<recob::PFParticle> partpv;
  art::PtrVector<recob::PCAxis>     pcapv;
}

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

template class std::vector<recob::Cluster>;
template class std::vector<recob::Hit>;
template class std::vector<recob::Seed>;
template class std::vector<recob::Shower>;
template class std::vector<recob::Track>;
template class std::vector<recob::EndPoint2D>;
template class std::pair<unsigned int, std::vector<float> >;
template class std::vector<std::pair<unsigned int, std::vector<float> > >;
template class std::vector<recob::Wire>;
template class std::vector<recob::SpacePoint>;
template class std::vector<recob::Vertex>;
template class std::vector<recob::Event>;
template class std::vector<recob::OpHit>;
template class std::vector<recob::OpFlash>;
template class std::vector<recob::PFParticle>;
template class std::vector<recob::PCAxis>;

template class art::Ptr<recob::Cluster>;
template class art::PtrVector<recob::Cluster>;

template class art::Ptr<recob::PCAxis>;
template class art::PtrVector<recob::PCAxis>;

template class art::Ptr<recob::PFParticle>;
template class art::PtrVector<recob::PFParticle>;

template class art::Ptr<recob::Seed>;
template class art::PtrVector<recob::Seed>;

template class art::Ptr<recob::Shower>;
template class art::PtrVector<recob::Shower>;

template class art::Ptr<recob::SpacePoint>;
template class art::PtrVector<recob::SpacePoint>;

template class art::Ptr<recob::OpHit>;
template class art::PtrVector<recob::OpHit>;

template class art::Ptr<recob::OpFlash>;
template class art::PtrVector<recob::OpFlash>;

template class std::pair< art::Ptr<recob::Cluster>,      art::Ptr<recob::EndPoint2D> >;
template class std::pair< art::Ptr<recob::Cluster>,      art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::Cluster>,      art::Ptr<recob::Shower>     >;
template class std::pair< art::Ptr<recob::Cluster>,      art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<recob::Cluster>,      art::Ptr<recob::SpacePoint> >;
template class std::pair< art::Ptr<recob::Cluster>,      art::Ptr<recob::Track>      >;
template class std::pair< art::Ptr<recob::Cluster>,      art::Ptr<recob::Vertex>     >;
template class std::pair< art::Ptr<recob::EndPoint2D>,   art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::EndPoint2D>,   art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::Event>,        art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::Event>,        art::Ptr<recob::Vertex>     >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::EndPoint2D> >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::Event>      >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::Track>      >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::Seed>       >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::Shower>     >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::SpacePoint> >;
template class std::pair< art::Ptr<recob::Hit>,          art::Ptr<recob::Vertex>     >;
template class std::pair< art::Ptr<recob::PCAxis>,       art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<recob::PFParticle>,   art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::PFParticle>,   art::Ptr<recob::PCAxis>     >;
template class std::pair< art::Ptr<recob::PFParticle>,   art::Ptr<recob::Seed>       >;
template class std::pair< art::Ptr<recob::PFParticle>,   art::Ptr<recob::Shower>     >;
template class std::pair< art::Ptr<recob::PFParticle>,   art::Ptr<recob::SpacePoint> >;
template class std::pair< art::Ptr<recob::PFParticle>,   art::Ptr<recob::Track>      >;
template class std::pair< art::Ptr<recob::PFParticle>,   art::Ptr<recob::Vertex>     >;
template class std::pair< art::Ptr<recob::Seed>,         art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::Seed>,         art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<recob::Shower>,       art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::Shower>,       art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::Shower>,       art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<recob::Shower>,       art::Ptr<recob::SpacePoint> >;
template class std::pair< art::Ptr<recob::Shower>,       art::Ptr<recob::Vertex>     >;
template class std::pair< art::Ptr<recob::SpacePoint>,   art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::SpacePoint>,   art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::SpacePoint>,   art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<recob::SpacePoint>,   art::Ptr<recob::Track>      >;
template class std::pair< art::Ptr<recob::SpacePoint>,   art::Ptr<recob::Shower>     >;
template class std::pair< art::Ptr<recob::Track>,        art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::Track>,        art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::Track>,        art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<recob::Track>,        art::Ptr<recob::SpacePoint> >;
template class std::pair< art::Ptr<recob::Track>,        art::Ptr<recob::Vertex>     >;
template class std::pair< art::Ptr<recob::OpFlash>,      art::Ptr<recob::OpHit>      >;
template class std::pair< art::Ptr<recob::OpFlash>,      art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::OpFlash>,      art::Ptr<recob::Track>      >;
template class std::pair< art::Ptr<recob::Vertex>,       art::Ptr<recob::Cluster>    >;
template class std::pair< art::Ptr<recob::Vertex>,       art::Ptr<recob::Hit>        >;
template class std::pair< art::Ptr<recob::Vertex>,       art::Ptr<recob::PFParticle> >;
template class std::pair< art::Ptr<recob::Vertex>,       art::Ptr<recob::Shower>     >;
template class std::pair< art::Ptr<recob::Vertex>,       art::Ptr<recob::Track>      >;
template class std::pair< art::Ptr<recob::Vertex>,       art::Ptr<recob::Event>      >;

template class art::Assns<raw::RawDigit,     recob::Wire,       void>;
template class art::Assns<raw::RawDigit,     recob::Hit,        void>;
template class art::Assns<recob::Wire,       raw::RawDigit,     void>;
template class art::Assns<recob::Wire,       recob::Hit,        void>;
template class art::Assns<recob::Cluster,    recob::EndPoint2D, void>;
template class art::Assns<recob::Cluster,    recob::EndPoint2D, unsigned short>;
template class art::Assns<recob::Cluster,    recob::Hit,        void>;
template class art::Assns<recob::Cluster,    recob::Shower,     void>;
template class art::Assns<recob::Cluster,    recob::SpacePoint, void>;
template class art::Assns<recob::Cluster,    recob::Track,      void>;
template class art::Assns<recob::Cluster,    recob::PFParticle, void>;
template class art::Assns<recob::Cluster,    recob::Vertex,     void>;
template class art::Assns<recob::Cluster,    recob::Vertex,     unsigned short>;
template class art::Assns<recob::EndPoint2D, recob::Cluster,    void>;
template class art::Assns<recob::EndPoint2D, recob::Cluster,    unsigned short>;
template class art::Assns<recob::EndPoint2D, recob::Hit,        void>;
template class art::Assns<recob::Event,      recob::Hit,        void>;
template class art::Assns<recob::Event,      recob::Vertex,     void>;
template class art::Assns<recob::Hit,        raw::RawDigit,     void>;
template class art::Assns<recob::Hit,        recob::Wire,       void>;
template class art::Assns<recob::Hit,        recob::Cluster,    void>;
template class art::Assns<recob::Hit,        recob::EndPoint2D, void>;
template class art::Assns<recob::Hit,        recob::Event,      void>;
template class art::Assns<recob::Hit,        recob::Seed,       void>;
template class art::Assns<recob::Hit,        recob::Shower,     void>;
template class art::Assns<recob::Hit,        recob::SpacePoint, void>;
template class art::Assns<recob::Hit,        recob::Track,      void>;
template class art::Assns<recob::PCAxis,     recob::PFParticle, void>;
template class art::Assns<recob::PFParticle, recob::Cluster,    void>;
template class art::Assns<recob::PFParticle, recob::PCAxis,     void>;
template class art::Assns<recob::PFParticle, recob::Seed,       void>;
template class art::Assns<recob::PFParticle, recob::Shower,     void>;
template class art::Assns<recob::PFParticle, recob::SpacePoint, void>;
template class art::Assns<recob::PFParticle, recob::Track,      void>;
template class art::Assns<recob::PFParticle, recob::Vertex,     void>;
template class art::Assns<recob::Hit,        recob::Vertex,     void>;
template class art::Assns<recob::Seed,       recob::Hit,        void>;
template class art::Assns<recob::Seed,       recob::PFParticle, void>;
template class art::Assns<recob::Shower,     recob::Cluster,    void>;
template class art::Assns<recob::Shower,     recob::Hit,        void>;
template class art::Assns<recob::Shower,     recob::PFParticle, void>;
template class art::Assns<recob::Shower,     recob::SpacePoint, void>;
template class art::Assns<recob::Shower,     recob::Vertex,     void>;
template class art::Assns<recob::SpacePoint, recob::Hit,        void>;
template class art::Assns<recob::SpacePoint, recob::Cluster,    void>;
template class art::Assns<recob::SpacePoint, recob::PFParticle, void>;
template class art::Assns<recob::SpacePoint, recob::Track,      void>;
template class art::Assns<recob::SpacePoint, recob::Shower,     void>;
template class art::Assns<recob::Track,      recob::Cluster,    void>;
template class art::Assns<recob::Track,      recob::Hit,        void>;
template class art::Assns<recob::Track,      recob::PFParticle, void>;
template class art::Assns<recob::Track,      recob::SpacePoint, void>;
template class art::Assns<recob::Track,      recob::Vertex,     void>;
template class art::Assns<recob::OpFlash,    recob::OpHit,      void>;
template class art::Assns<recob::OpFlash,    recob::Track,      void>;
template class art::Assns<recob::OpFlash,    recob::Cluster,    void>;
template class art::Assns<recob::Vertex,     recob::Cluster,    void>;
template class art::Assns<recob::Vertex,     recob::Cluster,    unsigned short>;
template class art::Assns<recob::Vertex,     recob::Event,      void>;
template class art::Assns<recob::Vertex,     recob::Hit,        void>;
template class art::Assns<recob::Vertex,     recob::PFParticle, void>;
template class art::Assns<recob::Vertex,     recob::Shower,     void>;
template class art::Assns<recob::Vertex,     recob::Track,      void>;

template class art::Wrapper< art::Assns<raw::RawDigit,     recob::Wire,       void> >;
template class art::Wrapper< art::Assns<raw::RawDigit,     recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Wire,       raw::RawDigit,     void> >;
template class art::Wrapper< art::Assns<recob::Wire,       recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::EndPoint2D, void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::EndPoint2D, unsigned short> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::Shower,     void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::SpacePoint, void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::Vertex,     void> >;
template class art::Wrapper< art::Assns<recob::Cluster,    recob::Vertex,     unsigned short> >;
template class art::Wrapper< art::Assns<recob::EndPoint2D, recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::EndPoint2D, recob::Cluster,    unsigned short> >;
template class art::Wrapper< art::Assns<recob::EndPoint2D, recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Event,      recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Event,      recob::Vertex,     void> >;
template class art::Wrapper< art::Assns<recob::Hit,        raw::RawDigit,     void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::Wire,       void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::EndPoint2D, void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::Event,      void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::Seed,       void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::Shower,     void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::SpacePoint, void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::Hit,        recob::Vertex,     void> >;
template class art::Wrapper< art::Assns<recob::PCAxis,     recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, recob::PCAxis,     void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, recob::Seed,       void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, recob::Shower,     void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, recob::SpacePoint, void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::PFParticle, recob::Vertex,     void> >;
template class art::Wrapper< art::Assns<recob::Seed,       recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Seed,       recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<recob::Shower,     recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::Shower,     recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Shower,     recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<recob::Shower,     recob::SpacePoint, void> >;
template class art::Wrapper< art::Assns<recob::Shower,     recob::Vertex,     void> >;
template class art::Wrapper< art::Assns<recob::SpacePoint, recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::SpacePoint, recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::SpacePoint, recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::SpacePoint, recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<recob::SpacePoint, recob::Shower,     void> >;
template class art::Wrapper< art::Assns<recob::Track,      recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::Track,      recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Track,      recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<recob::Track,      recob::SpacePoint, void> >;
template class art::Wrapper< art::Assns<recob::Track,      recob::Vertex,     void> >;
template class art::Wrapper< art::Assns<recob::OpFlash,    recob::OpHit,      void> >;
template class art::Wrapper< art::Assns<recob::OpFlash,    recob::Track,      void> >;
template class art::Wrapper< art::Assns<recob::OpFlash,    recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::Vertex,     recob::Cluster,    void> >;
template class art::Wrapper< art::Assns<recob::Vertex,     recob::Cluster,    unsigned short> >;
template class art::Wrapper< art::Assns<recob::Vertex,     recob::Hit,        void> >;
template class art::Wrapper< art::Assns<recob::Vertex,     recob::Event,      void> >;
template class art::Wrapper< art::Assns<recob::Vertex,     recob::PFParticle, void> >;
template class art::Wrapper< art::Assns<recob::Vertex,     recob::Shower,     void> >;
template class art::Wrapper< art::Assns<recob::Vertex,     recob::Track,      void> >;

template class art::Wrapper< std::vector<recob::Cluster>                     >;
template class art::Wrapper< std::vector<recob::Hit>                         >;
template class art::Wrapper< std::vector<recob::PCAxis>             	     >;
template class art::Wrapper< std::vector<recob::PFParticle>                  >;
template class art::Wrapper< std::vector<recob::Seed>               	     >;
template class art::Wrapper< std::vector<recob::Shower>             	     >;
template class art::Wrapper< std::vector<recob::Track>              	     >;
template class art::Wrapper< std::vector<recob::EndPoint2D>         	     >;
template class art::Wrapper< std::vector<recob::Wire>               	     >;
template class art::Wrapper< std::vector<recob::SpacePoint>         	     >;
template class art::Wrapper< std::vector<recob::Vertex>                      >;
template class art::Wrapper< std::vector<recob::Event>                	     >;
template class art::Wrapper< std::vector<recob::OpHit>                	     >;
template class art::Wrapper< std::vector<recob::OpFlash>                     >;
template class art::Wrapper< std::vector<art::PtrVector<recob::Cluster> >    >;
template class art::Wrapper< std::vector<art::PtrVector<recob::SpacePoint> > >;
template class art::Wrapper< std::vector<art::PtrVector<recob::Track>      > >;
