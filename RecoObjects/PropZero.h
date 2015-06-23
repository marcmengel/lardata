////////////////////////////////////////////////////////////////////////
///
/// \file   PropZero.h
///
/// \brief  Zero-distance propagator between any two, possibly dissimilar,
///         surfaces.
///
/// \author H. Greenlee 
///
/// Class for propagating zero distance between two dissimilar surfaces.
/// Return propagation failure if the distance to the destination surface
/// exceeds some tolerance.
///
/// This class may be called via Propagator base class methods other than
/// short_vec_prop (in particular, it makes sense to call method err_prop
/// to transform the error matrix between two surfaces).  All such calls
/// should have the dedx flag set to false.
///
////////////////////////////////////////////////////////////////////////

#ifndef PROPZERO_H
#define PROPZERO_H

#include "RecoObjects/Propagator.h"

namespace trkf {

  class PropZero : public trkf::Propagator
  {
  public:

    /// Constructor.
    PropZero(double max_dist = 1.e-3);

    /// Destructor.
    virtual ~PropZero();

    // Overrides.

    /// Clone method.
    Propagator* clone() const {return new PropZero(*this);}

    /// Propagate without error.
    boost::optional<double> short_vec_prop(KTrack& trk,
					   const std::shared_ptr<const Surface>& surf, 
					   Propagator::PropDirection dir, 
					   bool doDedx,
					   TrackMatrix* prop_matrix = 0,
					   TrackError* noise_matrix = 0) const;

    // Private member functions.

  private:

    // The following methods transform the track to the destination surface
    // without propagation.  Return bool value indicates success or failure.

    bool transformYZPlane_YZPlane(double y01, double z01, double phi1,
    				  double y02, double z02, double phi2,
    				  TrackVector& vec, Surface::TrackDirection& dir,
    				  TrackMatrix* prop_matrix) const;

    bool transformYZPlane_XYZPlane(double y01, double z01, double phi1,
				   double x02, double y02, double z02,
				   double theta2, double phi2,
				   TrackVector& vec, Surface::TrackDirection& dir,
				   TrackMatrix* prop_matrix) const;
		   
    bool transformXYZPlane_YZPlane(double x01, double y01, double z01,
				   double theta1, double phi1,
				   double y02, double z02, double phi2,
				   TrackVector& vec, Surface::TrackDirection& dir,
				   TrackMatrix* prop_matrix) const;
		   
    bool transformXYZPlane_XYZPlane(double x01, double y01, double z01,
				    double theta1, double phi1,
				    double x02, double y02, double z02,
				    double theta2, double phi2,
				    TrackVector& vec, Surface::TrackDirection& dir,
				    TrackMatrix* prop_matrix) const;
		   
    // Data members.

  private:

    double fMaxDist;   // Maximum perpendicular distance from initial position to destination.
  };
}

#endif
