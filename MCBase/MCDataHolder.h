#ifndef MCDATAHOLDER_H
#define MCDATAHOLDER_H

#include <algorithm>
#include <vector>
#include "MCLimits.h"

namespace sim {

  class MCEnDep {
    
  public:

    MCEnDep() { Reset(); }
    ~MCEnDep() {}

  protected:

    std::vector<double> fVertex;
    double              fEnergy;
    unsigned int        fTrackId;

  public:

    void Reset()
    {
      fVertex.resize(3,::sim::kINVALID_DOUBLE);
      fEnergy   = ::sim::kINVALID_DOUBLE;
      fTrackId  = ::sim::kINVALID_UINT;
    }

    void SetVertex(double x, double y, double z)
    {
      fVertex.at(0) = x;
      fVertex.at(1) = y;
      fVertex.at(2) = z;
    }

    void SetEnergy(double e) { fEnergy = e; }

    void SetTrackId(unsigned int id) { fTrackId = id; }

    const std::vector<double>& Vertex() const { return fVertex; }
    
    double Energy() const { return fEnergy; }

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
