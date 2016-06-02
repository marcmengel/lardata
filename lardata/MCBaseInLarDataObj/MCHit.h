
#ifndef MCHIT_H
#define MCHIT_H

// C++ includes
#include <iostream>
#include <sstream>
#include <algorithm>
#include "MCBaseException.h"
#include "MCLimits.h"

namespace sim {
  
  class MCHit {

  public:

    /// Default ctor
    MCHit()
    { 
      Reset();
    }

    /// Method to reset
    void Reset()
    {
      fSignalTime = fSignalWidth = ::sim::kINVALID_FLOAT;
      fPeakAmp = fCharge =  ::sim::kINVALID_FLOAT;
      for(int i=0; i<3; ++i)
	fPartVertex[i] = ::sim::kINVALID_FLOAT;
      fPartEnergy  =  ::sim::kINVALID_FLOAT;
      fPartTrackId =  ::sim::kINVALID_INT;
    }

  private:

    //
    // MCHit core information
    //
    float fSignalTime;     ///< where peak resides in waveform ticks
    float fSignalWidth;    ///< width (1sigma) in waveform ticks

    float fPeakAmp;        ///< Peak amplitude (ADC)
    float fCharge;         ///< Charge sum (ADC integral over MCWire)

    // 
    // Particle information that caused this MCHit
    //

    float fPartVertex[3];  ///< particle vertex (x,y,z) information
    float fPartEnergy;     ///< particle energy deposition (dE) in MeV
    int fPartTrackId;      ///< particle G4 Track ID

#ifndef __GCCXML__

  public:
    
    /// Setter function for charge/amplitude
    void SetCharge(float qsum, float amp) { fCharge=qsum; fPeakAmp=amp; }

    /// Setter function for time
    void SetTime(const float peak, const float width)
    {
      fSignalTime = peak;
      fSignalWidth = width;
    }

    /// Setter function for partile info
    void SetParticleInfo(const float vtx[], 
			 const float energy,
			 const int trackId)
    {
      for(size_t i=0; i<3; ++i)
	fPartVertex[i] = vtx[i];
      fPartEnergy  = energy;
      fPartTrackId = trackId;
    }

    /// Getter for start time
    float PeakTime()  const { return fSignalTime; }

    /// Getter for start time
    float PeakWidth()  const { return fSignalWidth; }

    /// Getter for "charge"
    float Charge(bool max=false) const { return ( max ? fPeakAmp : fCharge ); }

    /// Getter for particle vertex
    const float* PartVertex() const { return fPartVertex; }

    /// Getter for particle energy
    float PartEnergy() const { return fPartEnergy; }

    /// Getter for track ID
    int PartTrackId() const { return fPartTrackId; }

    /// For sorting with MCHit itself
    inline bool operator< ( const MCHit& rhs ) const { return fSignalTime < rhs.fSignalTime; }

    /// For sorting with generic time
    inline bool operator< ( const float& rhs) const { return fSignalTime < rhs; }

#endif
  };

}

// Define a pointer comparison
#ifndef __GCCXML__
namespace std {
  template <>
  class less<sim::MCHit*>
  {
  public:
    bool operator()( const sim::MCHit* lhs, const sim::MCHit* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif 
