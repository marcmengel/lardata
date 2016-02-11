////////////////////////////////////////////////////////////////////////
///
/// \file   SurfWireLine.h
///
/// \brief  Linear surface defined by wire id and x coordinate.
///
/// \author H. Greenlee 
///
/// This class derives from SurfYZLine.  This class does not add any
/// new members, but has a constructor that allows construction from
/// a wire id and x coordinate.
///
////////////////////////////////////////////////////////////////////////

#ifndef SURFWIREX_H
#define SURFWIREX_H

#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "lardata/RecoObjects/SurfYZLine.h"

namespace trkf {

  class SurfWireLine : public SurfYZLine
  {
  public:

    /// Constructor.
    SurfWireLine(const geo::WireID& wireid, double x);

    /// Destructor.
    virtual ~SurfWireLine();
  };
}

#endif
