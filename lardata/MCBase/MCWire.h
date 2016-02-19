
#ifndef MCWIRE_H
#define MCWIRE_H

// C++ includes
#include <vector>
#include <algorithm>
#include "MCLimits.h"

namespace sim {
  
  class MCWire : public std::vector<double> {

  public:

    /// Default ctor
    MCWire()
    {
      Reset();
    }

    void Reset()
    {
      std::vector<double>::clear();
      fStartTDC = sim::kINVALID_UINT;
    }

  private:

    unsigned int fStartTDC;

#ifndef __GCCXML__

  public:

    MCWire(const unsigned int start,
	   const std::vector<double> &wf)
    {
      SetStartTDC(start);
      SetWaveform(wf);
    }
    
    /// Setter function for time
    void SetStartTDC(const unsigned int start)
    {
      fStartTDC = start;
    }

    /// Setter function for waveform
    void SetWaveform(const std::vector<double>& wf)
    {
      this->resize(wf.size(),0);
      for(std::size_t i=0; i<wf.size(); ++i)
	this->at(i) = wf.at(i);
    }

    /// Getter for start time
    unsigned int StartTDC()  const { return fStartTDC;  }

    /// For sorting
    inline bool operator< ( const MCWire& rhs ) const { return fStartTDC < rhs.fStartTDC; }
    
#endif
  };
}

// Define a pointer comparison
#ifndef __GCCXML__
namespace std {
  template <>
  class less<sim::MCWire*>
  {
  public:
    bool operator()( const sim::MCWire* lhs, const sim::MCWire* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif 
