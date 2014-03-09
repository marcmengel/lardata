////////////////////////////////////////////////////////////////////////////
// \version $Id: Hit.h,v 1.7 2010/03/01 21:32:44 bpage Exp $
//
// \brief Definition of Hit object for LArSoft
//
// \author mitchell.soderberg@yale.edu
//
////////////////////////////////////////////////////////////////////////////
#ifndef HIT_H
#define HIT_H

#ifndef __GCCXML__
#include <iosfwd>
#endif

#include <vector>
#include <stdint.h>

#include "art/Persistency/Common/Ptr.h"

#include "RecoBase/Wire.h"
#include "RawData/RawDigit.h"

namespace recob {

  /// hits are 2D representations of charge deposited in the tdc/wire plane
  /// hits are assumed to be made from deconvoluted, unipolar signals
  class Hit {
  public:
      Hit(); // Default constructor

      std::vector<double>   fHitSignal;        ///< vector of ADC values within the hit window

  private:

      double                  fStartTime;      ///< initial tdc tick for hit		       
      double                  fSigmaStartTime; ///< uncertainty on initial tick		      
      double                  fEndTime;        ///< final tdc tick for hit		      
      double                  fSigmaEndTime;   ///< uncertainty on final tick		      
      double                  fPeakTime;       ///< tdc for the peak charge deposition	      
      double                  fSigmaPeakTime;  ///< uncertainty for tdc of the peak	      
      double                  fCharge;         ///< total charge deposited for hit	      
      double                  fMaxCharge;      ///< maximum ADC value in hit window	      
      double                  fSigmaCharge;    ///< uncertainty in total charge deposited	      
      double                  fSigmaMaxCharge; ///< maximum ADC value in hit window	      
      int                     fMultiplicity;   ///< how many hits could this one be shared with 
      double                  fGoodnessOfFit;  ///< how well do we believe we know this hit?    
      art::Ptr<recob::Wire>   fWire;           ///< index of Wire object this Hit was created on
      art::Ptr<raw::RawDigit> fRawDigit;       ///< index of RawDigit object this Hit was created on
      geo::View_t             fView;           ///< view for the plane of the hit
      geo::SigType_t          fSignalType;     ///< signal type for the plane of the hit
      geo::WireID	      fWireID;         ///< WireID for the hit (Cryostat, TPC, Plane, Wire)

#ifndef __GCCXML__

  public:
      Hit(geo::View_t view,
	  geo::SigType_t sigt,
	  geo::WireID wid,
	  double startTime, double sigmaStartTime,
	  double endTime,   double sigmaEndTime,
	  double peakTime,  double sigmaPeakTime,
	  double totcharge, double sigmaTotCharge,
	  double maxcharge, double sigmaMaxCharge,
	  int    multiplicity,
	  double goodnessOfFit);

      Hit(art::Ptr<recob::Wire> &wire,
	  geo::WireID wid,
	  double startTime, double sigmaStartTime,
	  double endTime,   double sigmaEndTime,
	  double peakTime,  double sigmaPeakTime,
	  double totcharge, double sigmaTotCharge,
	  double maxcharge, double sigmaMaxCharge,
	  int    multiplicity,
	  double goodnessOfFit);

      Hit(art::Ptr<raw::RawDigit> rawdigit,
	  geo::View_t view,
	  geo::SigType_t signaltype,
	  geo::WireID wid,
	  double startTime, double sigmaStartTime,
	  double endTime,   double sigmaEndTime,
	  double peakTime,  double sigmaPeakTime,
	  double totcharge, double sigmaTotCharge,
	  double maxcharge, double sigmaMaxCharge,
	  int    multiplicity,
	  double goodnessOfFit);
      
      // Get Methods
      double                  StartTime()                 const;
      double                  EndTime()                   const;
      double                  PeakTime()                  const;
      double                  SigmaStartTime()            const;
      double                  SigmaEndTime()              const;
      double                  SigmaPeakTime()             const;
      int                     Multiplicity()              const;
      uint32_t                Channel()                   const;
      double                  Charge(bool max=false)      const;
      double                  SigmaCharge(bool max=false) const;
      double                  GoodnessOfFit()             const;

      geo::SigType_t          SignalType()                const;
      geo::View_t             View()                      const;
      art::Ptr<recob::Wire>   Wire()                      const;
      art::Ptr<raw::RawDigit> RawDigit()                  const;
      geo::WireID	      WireID()		      	  const;

      friend std::ostream&  operator << (std::ostream & o, const Hit & a);
      friend bool           operator <  (const Hit & a, const Hit & b);

#endif
    };
}

#ifndef __GCCXML__

inline double                  recob::Hit::StartTime()      const { return fStartTime;            }
inline double                  recob::Hit::EndTime()        const { return fEndTime;       	  }
inline double                  recob::Hit::PeakTime()       const { return fPeakTime;      	  }
inline double                  recob::Hit::SigmaStartTime() const { return fSigmaStartTime;	  }
inline double                  recob::Hit::SigmaEndTime()   const { return fSigmaEndTime;  	  }
inline double                  recob::Hit::SigmaPeakTime()  const { return fSigmaPeakTime; 	  }
inline int                     recob::Hit::Multiplicity()   const { return fMultiplicity;  	  }
inline uint32_t                recob::Hit::Channel()        const { return RawDigit()->Channel(); }
inline double                  recob::Hit::GoodnessOfFit()  const { return fGoodnessOfFit;        }
inline geo::SigType_t          recob::Hit::SignalType()     const { return fSignalType;    	  }
inline geo::View_t             recob::Hit::View()           const { return fView;          	  }
inline art::Ptr<recob::Wire>   recob::Hit::Wire()           const { return fWire;          	  }
inline art::Ptr<raw::RawDigit> recob::Hit::RawDigit()       const { return fRawDigit.isNonnull() ? fRawDigit : fWire->RawDigit();}
#endif

#endif
