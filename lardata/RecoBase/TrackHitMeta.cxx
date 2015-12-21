////////////////////////////////////////////////////////////////////////////////////
///
/// \file   TrackHitMetadata.h
///
/// \brief  Class to keep data related to recob::Hit associated with recob::Track.
///
/// \author R. Sulej
///
////////////////////////////////////////////////////////////////////////////////////

#include "lardata/RecoBase/TrackHitMeta.h"

namespace recob {

/// Default constructor.
TrackHitMeta::TrackHitMeta(unsigned int idx, double dx)
{
	fIndex = idx;
	fDx = dx;
}

std::ostream& operator<< (std::ostream & o, const TrackHitMeta & a)
{
    o << a.Index();
    
    return o;
}
    
bool operator < (const TrackHitMeta & a, const TrackHitMeta & b)
{
    return a.Index() < b.Index();
}

} // end namespace trkf
