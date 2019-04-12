////////////////////////////////////////////////////////////////////////
///
/// \file   InteractGeneral.h
///
/// \brief  Interactor for planar surfaces.
///
/// \author H. Greenlee
///
/// This class calculates propagation noise for tracks on any surface.
/// This class works by transforming tracks to a planar surface that is
/// normal to the track and calculating the noise matrix on that surface,
/// then transforming the noise matrix back to the original surface.
///
////////////////////////////////////////////////////////////////////////

#ifndef INTERACTGENERAL_H
#define INTERACTGENERAL_H

#include "lardata/RecoObjects/InteractPlane.h"
#include "lardata/RecoObjects/PropAny.h"

namespace trkf {

  class InteractGeneral : public trkf::Interactor
  {
  public:

    /// Constructor.
    InteractGeneral(double tcut);

    /// Destructor.
    virtual ~InteractGeneral();

    // Overrides.

    /// Clone method.
    Interactor* clone() const {return new InteractGeneral(*this);}

    /// Calculate noise matrix.
    virtual bool noise(const KTrack& trk, double s, TrackError& noise_matrix) const;

  private:

    // Data members.

    /// Plane interactor.
    InteractPlane fInteract;

    /// Propagator.
    PropAny fProp;
  };
}

#endif
