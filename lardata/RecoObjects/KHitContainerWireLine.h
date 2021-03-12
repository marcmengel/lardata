////////////////////////////////////////////////////////////////////////
///
/// \file   KHitContainerWireLine.h
///
/// \brief  A KHitContainer for KHitWireLine type measurements.
///
/// \author H. Greenlee
///
/// This class derives from KHitContainer.  It does not add any
/// attributes of its own, nor does it override any base class
/// methods.  It does add a method for filling the container from a
/// collection of recob::Hit objects.
///
////////////////////////////////////////////////////////////////////////

#ifndef KHITCONTAINERWIRELINE_H
#define KHITCONTAINERWIRELINE_H

#include "canvas/Persistency/Common/PtrVector.h"
#include "lardata/RecoObjects/KHitContainer.h"
#include "lardataobj/RecoBase/Hit.h"

namespace trkf {

  class KHitContainerWireLine : public KHitContainer {
    void fill(const detinfo::DetectorPropertiesData& detProp,
              const art::PtrVector<recob::Hit>& hits,
              int only_plane) override;
  };
}

#endif
