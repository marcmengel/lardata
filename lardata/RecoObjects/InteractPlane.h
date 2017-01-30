////////////////////////////////////////////////////////////////////////
///
/// \file   InteractPlane.h
///
/// \brief  Interactor for planar surfaces.
///
/// \author H. Greenlee 
///
/// This class calculates propagation noise for tracks on planar
/// surfaces.  In particular, this class works for any surface that
/// has a local Cartesian coordinate system in which the track
/// parameters are (u, v, u'=du/dw, v'=dv/dw, q/p).
///
////////////////////////////////////////////////////////////////////////

#ifndef INTERACTPLANE_H
#define INTERACTPLANE_H

#include "lardata/RecoObjects/Interactor.h"

namespace detinfo {
  class LArProperties;
  class DetectorProperties;
}

namespace trkf {

  class InteractPlane : public trkf::Interactor
  {
  public:

    /// Constructor.
    InteractPlane(double tcut);

    /// Destructor.
    virtual ~InteractPlane();

    // Overrides.

    /// Clone method.
    Interactor* clone() const {return new InteractPlane(*this);}

    /// Calculate noise matrix.
    virtual bool noise(const KTrack& trk, double s, TrackError& noise_matrix) const;
    virtual bool noise(const TrackState& trk, double s, recob::tracking::SMatrixSym55& noise_matrix) const override;
    template<typename T> bool noise(detinfo::LArProperties const * larprop, detinfo::DetectorProperties const * detprop,
				    double dudw, double dvdw, double pinv, double mass, double s, bool flipSign, T& noise_matrix) const;
  };
}

#endif
