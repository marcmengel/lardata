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

#ifndef __GCCXML__
#include <iosfwd>
#endif
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

    Cluster              operator +  (Cluster);
    friend std::ostream& operator << (std::ostream& o, const Cluster& c);
    friend bool          operator <  (const Cluster & a, const Cluster & b);

#endif
  };
}

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

#endif

#endif //CLUSTER_H
