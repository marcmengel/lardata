#ifndef MCDATAHOLDER_H
#define MCDATAHOLDER_H

#include <algorithm>
#include <array>
#include "MCLimits.h"

namespace sim {

  class MCEnDep {
    
  public:

    MCEnDep() { Reset(); }
    ~MCEnDep() {}

  protected:

    std::array<float,3> fVertex;
    float               fEnergy;
    unsigned int        fTrackId;

  public:

    void Reset()
    {
      fVertex.fill(::sim::kINVALID_FLOAT);
      fEnergy   = ::sim::kINVALID_FLOAT;
      fTrackId  = ::sim::kINVALID_UINT;
    }

    void SetVertex(float x, float y, float z)
    {
      fVertex.at(0) = x;
      fVertex.at(1) = y;
      fVertex.at(2) = z;
    }

    void SetEnergy(float e) { fEnergy = e; }

    void SetTrackId(unsigned int id) { fTrackId = id; }

    const std::array<float,3>& Vertex() const { return fVertex; }
    
    float Energy() const { return fEnergy; }

    unsigned int TrackId() const { return fTrackId; }

    inline bool operator< (const MCEnDep& e) const
    {
      if( fTrackId < e.fTrackId ) return true;
      if( fTrackId > e.fTrackId ) return false;

      if( fVertex.at(0) < e.fVertex.at(0) ) return true;
      if( fVertex.at(0) > e.fVertex.at(0) ) return false;

      if( fVertex.at(1) < e.fVertex.at(1) ) return true;
      if( fVertex.at(1) > e.fVertex.at(1) ) return false;

      if( fVertex.at(2) < e.fVertex.at(2) ) return true;
      if( fVertex.at(2) > e.fVertex.at(2) ) return false;

      if( fEnergy < e.fEnergy ) return true;
      if( fEnergy > e.fEnergy ) return false;

      return false;
    }
  };
}
  
namespace std {
  template <>
  class less<sim::MCEnDep*>
  {
  public:
    bool operator()( const sim::MCEnDep* lhs, const sim::MCEnDep* rhs )
    { return (*lhs) < (*rhs); }
  };
}

#endif
