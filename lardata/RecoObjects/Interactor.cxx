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
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

namespace trkf {

  /// Constructor.
  ///
  /// Arguments:
  ///
  /// tcut - Maximum delta ray energy.
  ///
  Interactor::Interactor(double tcut) :
    fTcut(tcut)
  {}

  /// Destructor.
  Interactor::~Interactor()
  {}

    /// Method to calculate updated momentum due to dE/dx.
  ///
  /// Arguments:
  ///
  /// pinv  - Initial inverse momentum (units c/GeV).
  /// mass  - Particle mass (GeV/c^2).
  /// s     - Path distance.
  /// deriv - Pointer to store derivative d(pinv2)/d(pinv1) if nonzero.
  ///
  /// Returns: Final inverse momentum (pinv2) + success flag.
  ///
  /// Failure is returned in case of range out.
  ///
  /// Inverse momentum can be signed (q/p).  Returned inverse momentum
  /// has the same sign as the input.
  ///
  /// In this method, we are solving the differential equation in
  /// terms of energy.
  ///
  /// dE/dx = -f(E)
  ///
  /// where f(E) is the stopping power returned by method
  /// LArProperties::Eloss.
  ///
  /// We expect that this method will be called exclusively for short
  /// distance propagation.  The differential equation is solved using
  /// the midpoint method using a single step, which requires two
  /// evaluations of f(E).
  ///
  /// dE = -s*f(E1)
  /// E2 = E1 - s*f(E1 + 0.5*dE)
  ///
  /// The derivative is calculated assuming E2 = E1 + constant, giving
  ///
  /// d(pinv2)/d(pinv1) = pinv2^3 E2 / (pinv1^3 E1).
  /// 
  ///
  boost::optional<double> Interactor::dedx_prop(double pinv, double mass,
						double s, double* deriv) const
  {
    // For infinite initial momentum, return with success status,
    // still infinite momentum.

    if(pinv == 0.)
      return boost::optional<double>(true, 0.);

    // Set the default return value to be uninitialized with value 0.

    boost::optional<double> result(false, 0.);

    // Get LAr service.

    auto const * detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

    // Calculate final energy.

    double p1 = 1./std::abs(pinv);
    double e1 = std::hypot(p1, mass);
    double de = -0.001 * s * detprop->Eloss(p1, mass, fTcut);
    double emid = e1 + 0.5 * de;
    if(emid > mass) {
      double pmid = std::sqrt(emid*emid - mass*mass);
      double e2 = e1 - 0.001 * s * detprop->Eloss(pmid, mass, fTcut);
      if(e2 > mass) {
	double p2 = std::sqrt(e2*e2 - mass*mass);
	double pinv2 = 1./p2;
	if(pinv < 0.)
	  pinv2 = -pinv2;

	// Calculation was successful, update result.

	result = boost::optional<double>(true, pinv2);

	// Also calculate derivative, if requested.

	if(deriv != 0)
	  *deriv = pinv2*pinv2*pinv2 * e2 / (pinv*pinv*pinv * e1);
      }
    }

    // Done.

    return result;
  }

} // end namespace trkf
