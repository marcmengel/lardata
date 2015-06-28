////////////////////////////////////////////////////////////////////////
///
/// \file   PropYZLine.h
///
/// \brief  Propagate to SurfYZLine surface.
///
/// \author H. Greenlee 
///
/// Class for propagating to a destionation SurfYZLine surface.
///
////////////////////////////////////////////////////////////////////////

#ifndef PROPYZLINE_H
#define PROPYZLINE_H

#include "RecoObjects/PropZero.h"

namespace trkf {

  class PropYZLine : public trkf::Propagator
  {
  public:

    /// Constructor.
    PropYZLine(double tcut, bool doDedx);

    /// Destructor.
    virtual ~PropYZLine();

    // Overrides.

    /// Clone method.
    Propagator* clone() const {return new PropYZLine(*this);}

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
