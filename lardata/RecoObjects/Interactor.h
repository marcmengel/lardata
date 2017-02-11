////////////////////////////////////////////////////////////////////////
///
/// \file   Interactor.h
///
/// \brief  Base class for Kalman filter track interactor.
///
/// \author H. Greenlee 
///
/// This class defined the general interface for calculating
/// propagation noise.
///
/// This class defined a single pure virtual method called "noise"
/// whose purpose is to calculate the propagation noise matrix
/// associated with the propagation of a track over a specified
/// distance.
///
////////////////////////////////////////////////////////////////////////

#ifndef INTERACTOR_H
#define INTERACTOR_H

#include "lardata/RecoObjects/KalmanLinearAlgebra.h"
#include "lardata/RecoObjects/KTrack.h"
#include "lardata/RecoObjects/TrackState.h"
#include "boost/optional.hpp"

namespace trkf {

  class Interactor
  {
  public:

    /// Constructor.
    Interactor(double tcut);

    /// Destructor.
    virtual ~Interactor();

    // Accessors.

    double getTcut() const {return fTcut;}

    // Virtual methods.

    /// Clone method.
    virtual Interactor* clone() const = 0;

    /// Calculate noise matrix.
    virtual bool noise(const KTrack& trk, double s, TrackError& noise_matrix) const = 0;

    /// Calculate noise matrix.
    virtual bool noise(const TrackState& trk, double s, recob::tracking::SMatrixSym55& noise_matrix) const { return false; }

    /// Method to calculate updated momentum due to dE/dx.
    boost::optional<double> dedx_prop(double pinv, double mass,
				      double s, double* deriv=0) const;
    
  private:

    // Attributes.

    double fTcut;  ///< Maximum delta ray energy for dE/dx.
  };
}

#endif
