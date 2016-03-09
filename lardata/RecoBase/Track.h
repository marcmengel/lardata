////////////////////////////////////////////////////////////////////////////
// \version $Id: Track.h,v 1.5 2010/02/15 20:32:46 brebel Exp $
//
// \brief Definition of track object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#ifndef TRACK_H
#define TRACK_H

#include <vector>

#include "TMatrixD.h"
#include "TVector3.h"


#ifndef __GCCXML__
#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "larcore/SimpleTypesAndConstants/PhysicalConstants.h"
#include <iosfwd>
#endif

namespace recob {

  class Track {

  public:

    //Default constructor
    Track();

  private:

  protected:

    std::vector<TVector3>               fXYZ;           ///< position of points along the track
    std::vector<TVector3>               fDir;           ///< direction at each point along the track
    std::vector<TMatrixD >              fCov;           ///< covariance matrix of positions
                                                        ///< possibly only end points are stored
    std::vector< std::vector <double> > fdQdx;          ///< charge deposition per unit length at points
                                                        ///< along track outer vector index is over view
    std::vector<double>                 fFitMomentum;   ///< momentum at start and end of track
                                                        ///< determined from a fit. Intermediate points
                                                        ///< can be added if desired
    int                                 fID;            ///< track's ID

#ifndef __GCCXML__

  public:

    // Bezier-like constructor
    Track(std::vector<TVector3>               const& xyz,
	  std::vector<TVector3>               const& dxdydz,
	  std::vector< std::vector <double> > dQdx = std::vector< std::vector<double> >(0),
	  std::vector<double>                 fitMomentum = std::vector<double>(2, util::kBogusD),
	  int                                 ID = -1);
    
    // Kalman-like constructor
    Track(std::vector<TVector3>        const& xyz,
	  std::vector<TVector3>        const& dxdydz,
	  std::vector<TMatrixD >       const& cov,
	  std::vector< std::vector <double> > dQdx = std::vector< std::vector<double> >(0),
	  std::vector<double>                 fitMomentum = std::vector<double>(2, util::kBogusD),
	  int                                 ID = -1);

    void            Extent(std::vector<double> &xyzStart,
			   std::vector<double> &xyzEnd)        const;
    void            Direction(double *dcosStart,
			      double *dcosEnd)                 const;
    /*
     * The following methods have been moved into larreco/RecoAlg/TrackUtils.h:
     * - ProjectedLength() has become lar::TrackProjectedLength()
     * - PitchInView() has become lar::TrackPitchInView()
     */
//    double          ProjectedLength(geo::View_t view)          const;
//    double          PitchInView(geo::View_t view,
//				size_t trajectory_point=0)     const;
    int             ID()                                       const;


    // A trajectory point is the combination of a position vector
    // and its corresponding direction vector
    size_t          NumberTrajectoryPoints()                    const;
    size_t          NumberCovariance()                          const;
    size_t          NumberFitMomentum()                         const;
    size_t          NumberdQdx(geo::View_t view=geo::kUnknown)  const;
    double          Length(size_t p=0)                          const;
    void            TrajectoryAtPoint(unsigned int  p,
				      TVector3     &pos,
				      TVector3     &dir)        const;
    const double&   DQdxAtPoint(unsigned int p,
				geo::View_t view=geo::kUnknown) const;
    const TVector3& DirectionAtPoint (unsigned int p)           const;
    const TVector3& LocationAtPoint  (unsigned int p)           const;
    const double&   MomentumAtPoint  (unsigned int p)           const;
    const TMatrixD& CovarianceAtPoint(unsigned int p)           const;

    const TVector3& Vertex()                                    const;
    const TVector3& End()                                     	const;
    const TVector3& VertexDirection()                         	const;
    const TVector3& EndDirection()                            	const;
    const TMatrixD& VertexCovariance()                        	const;
    const TMatrixD& EndCovariance()                           	const;
    const double&   VertexMomentum()                          	const;
    const double&   EndMomentum()                             	const;

    double          Theta()                                     const;
    double          Phi()                                       const;
    double          ZenithAngle()                               const;
    double          AzimuthAngle()                              const;

    // Calculate rotation matrices between global (x,y,z) and local (u,v,w)
    // coordinate systems based on track direction (fDir).
    // The local w-axis points along the track direction.
    void GlobalToLocalRotationAtPoint(unsigned int p, TMatrixD& rot) const;
    void LocalToGlobalRotationAtPoint(unsigned int p, TMatrixD& rot) const;

    friend bool     operator <   (const Track & a, const Track & b);

  protected:

    friend std::ostream& operator << (std::ostream& stream, Track const& a);

#endif
  };
}

#ifndef __GCCXML__

inline int             recob::Track::ID()                               const { return fID;                  }
inline size_t          recob::Track::NumberTrajectoryPoints()           const { return fDir.size();          }
inline size_t          recob::Track::NumberCovariance()                 const { return fCov.size();          }
inline size_t          recob::Track::NumberFitMomentum()                const { return fFitMomentum.size();  }
inline const TVector3& recob::Track::DirectionAtPoint (unsigned int p)  const { return fDir.at(p);           }
inline const TVector3& recob::Track::LocationAtPoint  (unsigned int p)  const { return fXYZ.at(p);           }
inline const double&   recob::Track::MomentumAtPoint  (unsigned int p)  const { return fFitMomentum.at(p);   }
inline const TMatrixD& recob::Track::CovarianceAtPoint(unsigned int p)  const { return fCov.at(p);           }
											      
inline const TVector3& recob::Track::Vertex()                           const { return fXYZ.front();         }
inline const TVector3& recob::Track::End()                              const { return fXYZ.back();          }
inline const TVector3& recob::Track::VertexDirection()                  const { return fDir.front();         }
inline const TVector3& recob::Track::EndDirection()                     const { return fDir.back();          }
inline const TMatrixD& recob::Track::VertexCovariance()                 const { return fCov.front();         }
inline const TMatrixD& recob::Track::EndCovariance()                    const { return fCov.back();          }
inline const double&   recob::Track::VertexMomentum()                   const { return fFitMomentum.front(); }
inline const double&   recob::Track::EndMomentum()                      const { return fFitMomentum.back();  }
inline double          recob::Track::Theta()                            const { return fDir.front().Theta(); }
inline double          recob::Track::Phi()                              const { return fDir.front().Phi();   }


#endif

#endif // TRACK_H
