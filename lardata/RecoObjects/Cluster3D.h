/**
 *  @version $Id: Cluster3D.h, v1.0 2015/01/24 00:00:00 usher Exp $
 *
 *  @brief Definition of utility objects for use in the 3D clustering for LArSoft
 *
 *         The objects defined in this module are intended for internal use by the
 *         3D clustering (see Cluster3D_module.cc in larreco/ClusterFinder). 
 *         These objects mostly contain volatile information and are not suitable
 *         for storage in the art event store
 *
 *  @author usher@slac.stanford.edu
 *
 */

#ifndef RECO_CLUSTER3D_H
#define RECO_CLUSTER3D_H

#ifndef __GCCXML__
#include <iosfwd>
#endif
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>

#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/RecoBase/Hit.h"

namespace reco {
    
// First define a container object to augment the sparse 2D hit information
class ClusterHit2D
{
public:
    
    ClusterHit2D();   // Default constructor
    
private:
    
    mutable unsigned  m_statusBits;   ///< Volatile status information of this 3D hit
    mutable double    m_docaToAxis;   ///< DOCA of hit at POCA to associated cluster axis
    mutable double    m_arcLenToPoca; ///< arc length to POCA along cluster axis
    double            m_xPosition;    ///< The x coordinate for this hit
    double            m_timeTicks;    ///< The time (in ticks) for this hit
    const recob::Hit& m_hit;          ///< Hit we are augmenting
    
#ifndef __GCCXML__
public:
    
    enum StatusBits { SHAREDINPAIR    = 0x00080000,
                      SHAREDINTRIPLET = 0x00040000,
                      USEDINPAIR      = 0x00008000,
                      USEDINTRIPLET   = 0x00004000,
                      SHAREDINCLUSTER = 0x00000200,
                      USEDINCLUSTER   = 0x00000100,
                      USED            = 0x00000001
                    };
    
    ClusterHit2D(unsigned          statusBits,
                 double            doca,
                 double            poca,
                 double            xPosition,
                 double            timeTicks,
                 const recob::Hit& recobHit);
    
    unsigned          getStatusBits()   const {return m_statusBits;}
    double            getDocaToAxis()   const {return m_docaToAxis;}
    double            getArcLenToPoca() const {return m_arcLenToPoca;}
    double            getXPosition()    const {return m_xPosition;}
    double            getTimeTicks()    const {return m_timeTicks;}
    const recob::Hit& getHit()          const {return m_hit;}
    
    void setStatusBit(unsigned bits)    const {m_statusBits   |= bits;}
    void clearStatusBits(unsigned bits) const {m_statusBits   &= ~bits;}
    void setDocaToAxis(double doca)     const {m_docaToAxis    = doca;}
    void setArcLenToPoca(double poca)   const {m_arcLenToPoca  = poca;}
    
    friend std::ostream& operator << (std::ostream& o, const ClusterHit2D& c);
    friend bool          operator <  (const ClusterHit2D & a, const ClusterHit2D & b);
    
#endif
};
    
// Now define an object with the recob::Hit information that will comprise the 3D cluster
class ClusterHit3D
{
public:
    
    ClusterHit3D();   // Default constructor
    
private:
    
    mutable size_t                         m_id;              ///< "id" of this hit (useful for indexing)
    mutable unsigned int                   m_statusBits;      ///< Volatile status information of this 3D hit
    mutable double                         m_position[3];     ///< position of this hit combination in world coordinates
    double                                 m_totalCharge;     ///< Sum of charges of all associated recob::Hits
    double                                 m_avePeakTime;     ///< Average peak time of all associated recob::Hits
    double                                 m_deltaPeakTime;   ///< Largest delta peak time of associated recob::Hits
    double                                 m_sigmaPeakTime;   ///< Quad sum of peak time sigmas
    double                                 m_overlapFraction; ///< Smallest pulse overlap fraction
    mutable double                         m_docaToAxis;      ///< DOCA to the associated cluster axis
    mutable double                         m_arclenToPoca;    ///< arc length along axis to DOCA point
    mutable std::vector<geo::WireID>       m_wireIDVector;    ///< Wire ID's for the planes making up hit
    std::vector<const reco::ClusterHit2D*> m_hitVector;       ///< Hits comprising this 3D hit
    
#ifndef __GCCXML__
public:
    
    enum StatusBits { REJECTEDHIT     = 0x80000000,           ///< Hit has been rejected for any reason
                      SKELETONHIT     = 0x10000000,           ///< Hit is a "skeleton" hit
                      EDGEHIT         = 0x20000000,           ///< Hit is an "edge" hit
                      SEEDHIT         = 0x40000000,           ///< Hit is part of Seed for track fits
                      MADESPACEPOINT  = 0x08000000,           ///< Hit has been made into Space Point
                      SKELETONPOSAVE  = 0x00100000,           ///< Skeleton hit position averaged
                      CLUSTERVISITED  = 0x00008000,           ///< "visited" by a clustering algorithm
                      CLUSTERNOISE    = 0x00004000,           ///< Labelled "noise" by a clustering algorithm
                      CLUSTERATTACHED = 0x00002000,           ///< attached to a cluster
                      CLUSTERSHARED   = 0x00001000,           ///< 3D hit has 2D hits shared between clusters
                      PATHCHECKED     = 0x00000800,           ///< Path checking algorithm has seen this hit
                      SELECTEDBYMST   = 0x00000100,           ///< Hit has been used in Cluster Splitting MST
                      PCAOUTLIER      = 0x00000080,           ///< Hit labelled outlier in PCA
                      HITINVIEW0      = 0x00000001,           ///< Hit contains 2D hit from view 0 (u plane)
                      HITINVIEW1      = 0x00000002,           ///< Hit contains 2D hit from view 1 (v plane)
                      HITINVIEW2      = 0x00000004            ///< Hit contains 2D hit from view 2 (w plane)
                    };
        
    
    ClusterHit3D(size_t                                        id,
                 unsigned int                                  statusBits,
                 const double*                                 position,
                 double                                        totalCharge,
                 double                                        avePeakTime,
                 double                                        deltaPeakTime,
                 double                                        sigmaPeakTime,
                 double                                        docaToAxis,
                 double                                        arclenToPoca,
                 double                                        overlapFraction,
                 const std::vector<geo::WireID>&               wireIDVec,
                 const std::vector<const reco::ClusterHit2D*>& hitVec);
    
    size_t                                        getID()              const {return m_id;}
    unsigned int                                  getStatusBits()      const {return m_statusBits;}
    const double*                                 getPosition()        const {return m_position;}
    double                                        getX()               const {return m_position[0];}
    double                                        getY()               const {return m_position[1];}
    double                                        getZ()               const {return m_position[2];}
    double                                        getTotalCharge()     const {return m_totalCharge;}
    double                                        getAvePeakTime()     const {return m_avePeakTime;}
    double                                        getDeltaPeakTime()   const {return m_deltaPeakTime;}
    double                                        getSigmaPeakTime()   const {return m_sigmaPeakTime;}
    double                                        getOverlapFraction() const {return m_overlapFraction;}
    double                                        getDocaToAxis()      const {return m_docaToAxis;}
    double                                        getArclenToPoca()    const {return m_arclenToPoca;}
    const std::vector<geo::WireID>&               getWireIDs()         const {return m_wireIDVector;}
    const std::vector<const reco::ClusterHit2D*>& getHits()            const {return m_hitVector;}
    
    bool bitsAreSet(const unsigned int& bitsToCheck)                   const {return m_statusBits & bitsToCheck;}

    void setID(const size_t& id)           const {m_id            = id;}
    void setStatusBit(unsigned bits)       const {m_statusBits   |= bits;}
    void clearStatusBits(unsigned bits)    const {m_statusBits   &= ~bits;}
    void setDocaToAxis(double doca)        const {m_docaToAxis    = doca;}
    void setArclenToPoca(double poca)      const {m_arclenToPoca  = poca;}
    void setWireID(const geo::WireID& wid) const;
    
    void setPosition(const double* pos) const {m_position[0] = pos[0]; m_position[1] = pos[1]; m_position[2] = pos[2];}

    const bool operator<(const reco::ClusterHit3D& other) const
    {
        if (m_position[2] != other.m_position[2]) return m_position[2] < other.m_position[2];
        else return m_position[0] < other.m_position[0];
    }

    const bool operator==(const reco::ClusterHit3D& other) const
    {
        return m_id == other.m_id;
    }
    
    friend std::ostream& operator << (std::ostream& o, const ClusterHit3D& c);
    //friend bool          operator <  (const ClusterHit3D & a, const ClusterHit3D & b);
    
#endif
};
    
// We also need to define a container for the output of the PCA Analysis
class PrincipalComponents
{
public:

    typedef std::vector<std::vector<double> > EigenVectors;
    
    PrincipalComponents();
    
private:
    
    bool           m_svdOK;              ///< SVD Decomposition was successful
    int            m_numHitsUsed;        ///< Number of hits in the decomposition
    double         m_eigenValues[3];     ///< Eigen values from SVD decomposition
    EigenVectors   m_eigenVectors;       ///< The three principle axes
    double         m_avePosition[3];     ///< Average position of hits fed to PCA
    mutable double m_aveHitDoca;         ///< Average doca of hits used in PCA
    
#ifndef __GCCXML__
public:
    
    PrincipalComponents(bool ok, int nHits, const double* eigenValues, const EigenVectors& eigenVecs, const double* avePos, const double aveHitDoca = 9999.);
    
    bool                getSvdOK()            const {return m_svdOK;}
    int                 getNumHitsUsed()      const {return m_numHitsUsed;}
    const double*       getEigenValues()      const {return m_eigenValues;}
    const EigenVectors& getEigenVectors()     const {return m_eigenVectors;}
    const double*       getAvePosition()      const {return m_avePosition;}
    const double        getAveHitDoca()       const {return m_aveHitDoca;}
    
    void                flipAxis(size_t axis);
    void                setAveHitDoca(double doca) const {m_aveHitDoca = doca;}
    
    friend std::ostream&  operator << (std::ostream & o, const PrincipalComponents& a);
    friend bool operator < (const PrincipalComponents& a, const PrincipalComponents& b);
    
#endif
};

class Cluster3D
{
public:

    Cluster3D();  ///Default constructor

private:

    mutable unsigned    m_statusBits;       ///< Status bits for the cluster
    PrincipalComponents m_pcaResults;       ///< Output of the prinicipal componenets analysis
    double              m_totalCharge;      ///< Total charge in the cluster
    double              m_startPosition[3]; ///< "start" position for cluster (world coordinates)
    double              m_endPosition[3];   ///< "end" position for cluster
    int                 m_clusterIdx;       ///< ID for this cluster
    

#ifndef __GCCXML__

public:
    Cluster3D(unsigned                   statusBits,
              const PrincipalComponents& pcaResults,
              double                     totalCharge,
              const double*              startPosition,
              const double*              endPosition,
              int                        idx);
    
    unsigned                   getStatusBits()    const {return m_statusBits;}
    const PrincipalComponents& getPcaResults()    const {return m_pcaResults;}
    double                     getTotalCharge()   const {return m_totalCharge;}
    const double*              getStartPosition() const {return m_startPosition;}
    const double*              getEndPosition()   const {return m_endPosition;}
    int                        getClusterIdx()    const {return m_clusterIdx;}
    
    void setStatusBit(unsigned bits)    const {m_statusBits |= bits;}
    void clearStatusBits(unsigned bits) const {m_statusBits &= ~bits;}
    
    Cluster3D            operator +  (Cluster3D);
    friend std::ostream& operator << (std::ostream& o, const Cluster3D& c);
    friend bool          operator <  (const Cluster3D & a, const Cluster3D & b);

#endif
};
    
typedef std::vector<const reco::ClusterHit2D*>             HitVectorConst;

/**
 *  @brief A utility class used in construction of 3D clusters
 */
class RecobClusterParameters
{
public:
    RecobClusterParameters() : m_startTime(999999.),
    m_sigmaStartTime(1.),
    m_endTime(0.),
    m_sigmaEndTime(1.),
    m_totalCharge(0.),
    m_startWire(9999999),
    m_endWire(0),
    m_view(geo::kUnknown)
    {
        m_hitVector.clear();
    }
    
    void UpdateParameters(const reco::ClusterHit2D* hit);
    
    double         m_startTime;
    double         m_sigmaStartTime;
    double         m_endTime;
    double         m_sigmaEndTime;
    double         m_totalCharge;
    unsigned int   m_startWire;
    unsigned int   m_endWire;
    geo::View_t    m_view;
    HitVectorConst m_hitVector;
};
    
    
/**
 *  @brief export some data structure definitions
 */
using Hit2DListPtr       = std::list<const reco::ClusterHit2D*>;
using HitPairListPtr     = std::list<const reco::ClusterHit3D*>;
using HitPairSetPtr      = std::set<const reco::ClusterHit3D*>;
using HitPairListPtrList = std::list<HitPairListPtr>;
using HitPairClusterMap  = std::map<int, HitPairListPtr>;
using HitPairList        = std::list<std::unique_ptr<reco::ClusterHit3D>>;
    
using PCAHitPairClusterMapPair = std::pair<reco::PrincipalComponents, reco::HitPairClusterMap::iterator>;
using ViewToClusterParamsMap   = std::map<geo::View_t, RecobClusterParameters>;
    
using EdgeTuple        = std::tuple<const reco::ClusterHit3D*,const reco::ClusterHit3D*,double>;
using EdgeList         = std::list<EdgeTuple>;
using Hit3DToEdgePair  = std::pair<const reco::ClusterHit3D*, reco::EdgeList>;
using Hit3DToEdgeMap   = std::unordered_map<const reco::ClusterHit3D*, reco::EdgeList>;
    

/**
 *  @brief Class wrapping the above and containing volatile information to characterize the cluster
 */
class ClusterParameters
{
public:
    ClusterParameters()
    {
        m_clusterParams.clear();
        m_hitPairListPtr.clear();
        m_hit3DToEdgeMap.clear();
        m_bestHitPairListPtr.clear();
        m_bestEdgeList.clear();
    }
    
    ClusterParameters(reco::HitPairClusterMap::iterator& mapItr) : m_hitPairListPtr(mapItr->second)
    {
        m_clusterParams.clear();
        m_hit3DToEdgeMap.clear();
        m_bestHitPairListPtr.clear();
        m_bestEdgeList.clear();
    }
    
    ClusterParameters(reco::HitPairListPtr& hitList) : m_hitPairListPtr(hitList)
    {
        m_clusterParams.clear();
        m_hit3DToEdgeMap.clear();
        m_bestHitPairListPtr.clear();
        m_bestEdgeList.clear();
    }
    
    void UpdateParameters(const reco::ClusterHit2D* hit)
    {
        m_clusterParams[hit->getHit().View()].UpdateParameters(hit);
    }
    
    reco::ViewToClusterParamsMap& getClusterParams()      {return m_clusterParams;}
    reco::HitPairListPtr&         getHitPairListPtr()     {return m_hitPairListPtr;}
    reco::PrincipalComponents&    getFullPCA()            {return m_fullPCA;}
    reco::PrincipalComponents&    getSkeletonPCA()        {return m_skeletonPCA;}
    reco::Hit3DToEdgeMap&         getHit3DToEdgeMap()     {return m_hit3DToEdgeMap;}
    reco::HitPairListPtr&         getBestHitPairListPtr() {return m_bestHitPairListPtr;}
    reco::EdgeList&               getBestEdgeList()       {return m_bestEdgeList;}
    
    friend bool operator < (const ClusterParameters &a, const ClusterParameters& b)
    {
        return a.m_hitPairListPtr.size() > b.m_hitPairListPtr.size();
    }

private:
    ViewToClusterParamsMap    m_clusterParams;
    reco::HitPairListPtr      m_hitPairListPtr;    // This contains the list of 3D hits in the cluster
    reco::PrincipalComponents m_fullPCA;
    reco::PrincipalComponents m_skeletonPCA;
    reco::Hit3DToEdgeMap      m_hit3DToEdgeMap;
    reco::HitPairListPtr      m_bestHitPairListPtr;
    reco::EdgeList            m_bestEdgeList;
};

using ClusterParametersList = std::list<ClusterParameters>;
    
}

#ifndef __GCCXML__

#endif

#endif //RECO_CLUSTER3D_H
