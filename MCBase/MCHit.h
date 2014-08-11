
#ifndef MCHIT_H
#define MCHIT_H

// C++ includes
#include <vector>
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
      fSignalTime = fSignalWidth = ::sim::kINVALID_DOUBLE;
      fPeakAmp = fCharge =  ::sim::kINVALID_DOUBLE;
      fPartVertex.resize(3, ::sim::kINVALID_DOUBLE);
      fPartEnergy  =  ::sim::kINVALID_DOUBLE;
      fPartTrackId =  ::sim::kINVALID_INT;
    }

  private:

    //
    // MCHit core information
    //
    double fSignalTime;  ///< where peak resides in waveform ticks
    double fSignalWidth; ///< width (1sigma) in waveform ticks

    double fPeakAmp; ///< Peak amplitude (ADC)
    double fCharge;  ///< Charge sum (ADC integral over MCWire)

    // 
    // Particle information that caused this MCHit
    //

    std::vector<double> fPartVertex; ///< particle vertex (x,y,z) information
    double fPartEnergy;              ///< particle energy deposition (dE) in MeV
    int fPartTrackId;                ///< particle G4 Track ID

#ifndef __GCCXML__

  public:
    
    /// Setter function for charge/amplitude
    void SetCharge(double qsum, double amp) { fCharge=qsum; fPeakAmp=amp; }

    /// Setter function for time
    void SetTime(const double peak, const double width)
    {
      fSignalTime = peak;
      fSignalWidth = width;
    }

    /// Setter function for partile info
    void SetParticleInfo(const std::vector<double>& vtx, 
			 const double energy,
			 const int trackId)
    {
      if(vtx.size()!=fPartVertex.size()) {

	std::ostringstream msg;
	msg << "<<" << __FUNCTION__ << ">>"  << " Invalid particle vtx length "
	    << vtx.size() << " != " << fPartVertex.size() << std::endl;
	
	throw ::sim::MCBaseException(msg.str());

      }

      for(size_t i=0; i<fPartVertex.size(); ++i) fPartVertex.at(i) = vtx.at(i);
      fPartEnergy  = energy;
      fPartTrackId = trackId;
    }

    /// Getter for start time
    double PeakTime()  const { return fSignalTime; }

    /// Getter for start time
    double PeakWidth()  const { return fSignalWidth; }

    /// Getter for "charge"
    double Charge(bool max=false) const { return ( max ? fPeakAmp : fCharge ); }

    /// Getter for particle vertex
    const std::vector<double>& PartVertex() const { return fPartVertex; }

    /// Getter for particle energy
    double PartEnergy() const { return fPartEnergy; }

    /// Getter for track ID
    int PartTrackId() const { return fPartTrackId; }

    /// For sorting with MCHit itself
    inline bool operator< ( const MCHit& rhs ) const { return fSignalTime < rhs.fSignalTime; }

    /// For sorting with generic time
    inline bool operator< ( const double& rhs) const { return fSignalTime < rhs; }

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
