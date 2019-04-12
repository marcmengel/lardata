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

#include "lardata/RecoObjects/KHitContainer.h"
#include "lardataobj/RecoBase/Hit.h"
#include "canvas/Persistency/Common/PtrVector.h"

namespace trkf {

  class KHitContainerWireX : public KHitContainer
  {
  public:

    /// Default constructor.
    KHitContainerWireX();

    /// Destructor.
    virtual ~KHitContainerWireX();

    /// Fill container.
    void fill(const art::PtrVector<recob::Hit>& hits, int only_plane) override;
  };
}

#endif
