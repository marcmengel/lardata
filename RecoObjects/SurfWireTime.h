////////////////////////////////////////////////////////////////////////
///
/// \file   SurfWireTime.h
///
/// \brief  Linear surface defined by wire id and drift time.
///
/// \author H. Greenlee 
///
/// This class derives from SurfYZLine.  This class does not add any
/// new members, but has a constructor that allows construction from
/// a wire id and drift time or recob::Hit.
///
////////////////////////////////////////////////////////////////////////

#ifndef SURFWIREX_H
#define SURFWIREX_H

#include "SimpleTypesAndConstants/geo_types.h"
#include "RecoObjects/SurfYZLine.h"

namespace trkf {

  class SurfWireTime : public SurfYZLine
  {
  public:

    /// Constructor.
    SurfWireTime(const geo::WireID& wireid, double time);

    /// Destructor.
    virtual ~SurfWireTime();
  };
}

#endif
