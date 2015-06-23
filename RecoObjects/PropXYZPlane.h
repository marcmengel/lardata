////////////////////////////////////////////////////////////////////////
///
/// \file   PropXYZPlane.h
///
/// \brief  Propagate between two SurfXYZPlanes.
///
/// \author H. Greenlee 
///
/// Class for propagating to a destionation SurfYZPlane surface.
///
////////////////////////////////////////////////////////////////////////

#ifndef PROPXYZPLANE_H
#define PROPXYZPLANE_H

#include "RecoObjects/PropZero.h"

namespace trkf {

  class PropXYZPlane : public trkf::Propagator
  {
  public:

    /// Constructor.
    PropXYZPlane(double tcut, bool doDedx);

    /// Destructor.
    virtual ~PropXYZPlane();

    // Overrides.

    /// Clone method.
    Propagator* clone() const {return new PropXYZPlane(*this);}

    /// Propagate without error.
    boost::optional<double> short_vec_prop(KTrack& trk,
					   const std::shared_ptr<const Surface>& surf, 
					   Propagator::PropDirection dir, 
					   bool doDedx,
					   TrackMatrix* prop_matrix = 0,
					   TrackError* noise_matrix = 0) const;

    // Data members.

  private:

    PropZero fPropZero;   // Zero distance propagator.
  };
}

#endif
