////////////////////////////////////////////////////////////////////////
///
/// \file   KHitWireLine.h
///
/// \brief  Kalman filter wire-time measurement on a SurfWireLine surface.
///
/// \author H. Greenlee
///
/// This class is a type of one-dimensional Kalman filter measurement
/// reprsenting a single wire-time hit on a line surface parallel to the
/// a readout wire corresponding to a specified hit or drift time.
///
/// This class derives from base class KHit<1>, which is the general
/// one-dimensional measurement base class.  This class has a constructor
/// from an art::Ptr<recob::Hit>, which pointer is retained as a data
/// product.  This class overrides virtual base class method subpredict.
///
///
////////////////////////////////////////////////////////////////////////

#ifndef KHITWIRELINE_H
#define KHITWIRELINE_H

#include "lardata/RecoObjects/KHit.h"
#include "lardataobj/RecoBase/Hit.h"
#include "canvas/Persistency/Common/Ptr.h"

namespace trkf {

  class KHitWireLine : public KHit<1>
  {
  public:

    /// Constructor from Hit.
    KHitWireLine(const art::Ptr<recob::Hit>& hit,
		 const std::shared_ptr<const Surface>& psurf);

    /// Constructor from wire id (mainly for testing).
    KHitWireLine(const geo::WireID& wireid, double x, double xerr);

    /// Destructor.
    virtual ~KHitWireLine();

    // Accessors.

    /// Get original hit.
    const art::Ptr<recob::Hit>& getHit() const {return fHit;}

    // Overrides.

    // Prediction method.
    virtual bool subpredict(const KETrack& tre,
			    KVector<1>::type& pvec,
			    KSymMatrix<1>::type& perr,
			    KHMatrix<1>::type& hmatrix) const;

  private:

    // Attributes.

    art::Ptr<recob::Hit> fHit;
  };
}

#endif
