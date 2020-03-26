///////////////////////////////////////////////////////////////////////
///
/// \file   Interactor.cxx
///
/// \brief  Base class for Kalman filter track interactor.
///
/// \author H. Greenlee
///
////////////////////////////////////////////////////////////////////////

#include "lardata/RecoObjects/Interactor.h"

namespace trkf {

  /// Constructor.
  ///
  /// Arguments:
  ///
  /// tcut - Maximum delta ray energy.
  ///
  Interactor::Interactor(double tcut) : fTcut(tcut) {}

  Interactor::~Interactor() = default;

} // end namespace trkf
