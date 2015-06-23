////////////////////////////////////////////////////////////////////////
///
/// \file   PropYZPlane.h
///
/// \brief  Propagate between two SurfYZPlanes.
///
/// \author H. Greenlee 
///
/// Class for propagating to a destionation SurfYZPlane surface.
///
////////////////////////////////////////////////////////////////////////

#ifndef PROPYZPLANE_H
#define PROPYZPLANE_H

#include "RecoObjects/PropZero.h"

namespace trkf {

  class PropYZPlane : public trkf::Propagator
  {
  public:

    /// Constructor.
    PropYZPlane(double tcut, bool doDedx);

    /// Destructor.
    virtual ~PropYZPlane();

    // Overrides.

    /// Clone method.
    Propagator* clone() const {return new PropYZPlane(*this);}

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
