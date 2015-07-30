///////////////////////////////////////////////////////////////////////
///
/// \file   SurfWireTime.cxx
///
/// \brief  Linear surface defined by wire id and drift time.
///
/// \author H. Greenlee
///
////////////////////////////////////////////////////////////////////////

#include "RecoObjects/SurfWireTime.h"
#include "Geometry/Geometry.h"
#include "Geometry/WireGeo.h"
#include "Utilities/DetectorProperties.h"
#include "TMath.h"

namespace trkf {

  /// Constructor.
  ///
  /// Arguments:
  ///
  /// wireid - Wire id.
  /// time   - Drift time (ticks).
  ///
  SurfWireTime::SurfWireTime(const geo::WireID& wireid, double time)
  {
    // Get services.

    art::ServiceHandle<geo::Geometry> geom;
    art::ServiceHandle<util::DetectorProperties> detprop;

    // Get wire geometry.

    geo::WireGeo const& wgeom = geom->WireIDToWireGeo(wireid);
	  
    // Get wire center and angle from the wire geometry.
    // Put local origin at center of wire.
	  
    double xyz[3] = {0.};
    wgeom.GetCenter(xyz);
    double phi = TMath::PiOver2() - wgeom.ThetaZ();

    // Get x coordinate.

    double x = detprop->ConvertTicksToX(time, wireid.Plane, wireid.TPC, wireid.Cryostat);    
	  
    // Update base class.
	  
    *static_cast<SurfYZLine*>(this) = SurfYZLine(x, xyz[1], xyz[2], phi);
  }

  /// Destructor.
  SurfWireTime::~SurfWireTime()
  {}

} // end namespace trkf
