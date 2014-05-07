////////////////////////////////////////////////////////////////////////////
// \version $Id: Cluster.h,v 1.6 2010/06/12 21:46:34 spitz7 Exp $
//
// \brief Definition of cluster object for LArSoft
//
// \author mitchell.soderberg@yale.edu
//
////////////////////////////////////////////////////////////////////////////

#ifndef CLUSTER_H
#define CLUSTER_H

#include <ostream>
#include <vector>

#include "SimpleTypesAndConstants/geo_types.h"

namespace recob {

  class Cluster {

  public:

    Cluster();  ///Default constructor

   private:

    double                     fTotalCharge;    ///< total charge in cluster
    double                     fdTdW;           ///< slope of cluster in tdc vs wire
    double                     fdQdW;           ///< slope of cluster in charge vs wire
    double                     fSigmadTdW;      ///< slope of cluster in tdc vs wire
    double                     fSigmadQdW;      ///< slope of cluster in charge vs wire
    std::vector<double>        fStartPos;       ///< start of cluster in (wire, tdc) plane
    std::vector<double>        fEndPos;         ///< start of cluster in (wire, tdc) plane
    std::vector<double>        fSigmaStartPos;  ///< start of cluster in (wire, tdc) plane
    std::vector<double>        fSigmaEndPos;    ///< start of cluster in (wire, tdc) plane
    int                        fID;             ///< cluster's ID
    geo::View_t                fView;           ///< view for this cluster
    geo::PlaneID               fPlaneID;        ///< location of the start of the cluster (cryo, tdc, plane)

#ifndef __GCCXML__

  public:
    Cluster(double startWire, double sigmaStartWire,
            double startTime, double sigmaStartTime,
            double endWire, double sigmaEndWire,
            double endTime, double sigmaEndTime,
            double dTdW, double sigmadTdW,
            double dQdW, double sigmadQdW,
            double totalQ,
            geo::View_t view,
            int id);
    Cluster(double startWire, double sigmaStartWire,
            double startTime, double sigmaStartTime,
            double endWire, double sigmaEndWire,
            double endTime, double sigmaEndTime,
            double dTdW, double sigmadTdW,
            double dQdW, double sigmadQdW,
            double totalQ,
            geo::View_t view,
            int id,
            const geo::PlaneID& planeID);
    
    //@{
    /// Accessors
    double                Charge()        const;
    geo::View_t           View()          const;
    double                dTdW()          const;
    double                dQdW()          const;
    double                SigmadTdW()     const;
    double                SigmadQdW()     const;
    std::vector<double>   StartPos()      const;
    std::vector<double>   EndPos()        const;
    std::vector<double>   SigmaStartPos() const;
    std::vector<double>   SigmaEndPos()   const;
    int                   ID()            const;
    const geo::PlaneID&   Plane()         const; ///< returns the geometry plane of the cluster
    //@}
    
    /// Returns whether geometry plane is valid
    bool                  hasPlane()      const;

    /// Moves the cluster to the specified plane
    Cluster& MoveToPlane(const geo::PlaneID& new_plane);
    
    /// Makes the plane of this cluster invalid
    Cluster& InvalidatePlane();
    
    Cluster              operator +  (const Cluster&);
    friend std::ostream& operator << (std::ostream& o, const Cluster& c);
    friend bool          operator <  (const Cluster & a, const Cluster & b);
    
#endif
  }; // class Cluster
} // namespace recob

#ifndef __GCCXML__

inline double                recob::Cluster::Charge()        const { return fTotalCharge;   }
inline geo::View_t           recob::Cluster::View()          const { return fView;          }
inline double                recob::Cluster::dTdW()          const { return fdTdW;          }
inline double                recob::Cluster::dQdW()          const { return fdQdW;          }
inline double                recob::Cluster::SigmadTdW()     const { return fSigmadTdW;     }
inline double                recob::Cluster::SigmadQdW()     const { return fSigmadQdW;     }
inline std::vector<double>   recob::Cluster::StartPos()      const { return fStartPos;      }
inline std::vector<double>   recob::Cluster::EndPos()        const { return fEndPos;        }
inline std::vector<double>   recob::Cluster::SigmaStartPos() const { return fSigmaStartPos; }
inline std::vector<double>   recob::Cluster::SigmaEndPos()   const { return fSigmaEndPos;   }
inline int                   recob::Cluster::ID()            const { return fID;            }
inline bool                  recob::Cluster::hasPlane()      const { return fPlaneID.isValid; }
inline const geo::PlaneID&   recob::Cluster::Plane()         const { return fPlaneID;       }

inline recob::Cluster&       recob::Cluster::MoveToPlane(const geo::PlaneID& new_plane)
  { fPlaneID = new_plane; return *this; }

inline recob::Cluster&       recob::Cluster::InvalidatePlane()
  { return MoveToPlane(geo::PlaneID()); }

#endif

#endif //CLUSTER_H
