////////////////////////////////////////////////////////////////////////
///
/// \file   KHitContainerWireX.h
///
/// \brief  A KHitContainer for KHitWireX type measurements.
///
/// \author H. Greenlee
///
/// This class derives from KHitContainer.  It does not add any
/// attributes of its own, nor does it override any base class
/// methods.  It does add a method for filling the container from a
/// collection of recob::Hit objects.
///
////////////////////////////////////////////////////////////////////////

#ifndef KHITCONTAINERWIREX_H
#define KHITCONTAINERWIREX_H

#include "canvas/Persistency/Common/PtrVector.h"
#include "lardata/RecoObjects/KHitContainer.h"
#include "lardataobj/RecoBase/Hit.h"

namespace trkf {

  class KHitContainerWireX : public KHitContainer {
  public:
    void fill(const detinfo::DetectorPropertiesData& clock_data,
              const art::PtrVector<recob::Hit>& hits,
              int only_plane) override;
  };
}

#endif
