////////////////////////////////////////////////////////////////////////
///
/// \file   PropAny.h
///
/// \brief  Propagate between any two surfaces.
///
/// \author H. Greenlee 
///
/// Class for propagating between any two surfaces.  This propagator
/// tests the type of the destination surface, and calls the
/// propagator.
///
////////////////////////////////////////////////////////////////////////

#ifndef PROPANY_H
#define PROPANY_H

#include "RecoObjects/Propagator.h"
#include "RecoObjects/PropYZPlane.h"
#include "RecoObjects/PropXYZPlane.h"

namespace trkf {

  class PropAny : public trkf::Propagator
  {
  public:

    /// Constructor.
    PropAny(double tcut, bool doDedx);

    /// Destructor.
    virtual ~PropAny();

    // Overrides.

    /// Clone method.
    Propagator* clone() const {return new PropAny(*this);}

    /// Propagate without error.
    boost::optional<double> short_vec_prop(KTrack& trk,
					   const std::shared_ptr<const Surface>& surf, 
					   Propagator::PropDirection dir, 
					   bool doDedx,
					   TrackMatrix* prop_matrix = 0,
					   TrackError* noise_matrix = 0) const;

    // Data members.

  private:

    /// Underlying propagators.

    PropYZPlane fPropYZPlane;
    PropXYZPlane fPropXYZPlane;
  };
}

#endif
