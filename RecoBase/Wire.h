////////////////////////////////////////////////////////////////////////
// \version $Id: Wire.h,v 1.7 2010/03/26 20:07:08 brebel Exp $
//
// \brief Definition of basic wire object.  The deconvoluted signals are stored in this class
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#ifndef WIRE_H
#define WIRE_H

#include <vector>
#include <stdint.h>

#include "RawData/RawDigit.h"
#include "SimpleTypesAndConstants/geo_types.h"

#include "art/Persistency/Common/Ptr.h"


///Reconstruction base classes
namespace recob {

  class Wire {
    public:
      typedef std::pair< unsigned int, std::vector<float> > InterestingRegion_t;
        ///< a region of interest is a pair (TDC offset, readings)
      typedef std::vector<InterestingRegion_t> RegionsOfInterest_t;
      
      Wire(); // Default constructor
      ~Wire();
      
    private:
      RegionsOfInterest_t fSignalROI;
      art::Ptr<raw::RawDigit> fRawDigit;   ///< vector to index of raw digit for this wire
      geo::View_t             fView;       ///< view corresponding to the plane of this wire
      geo::SigType_t          fSignalType; ///< signal type of the plane for this wire
      unsigned int            fMaxSamples; ///< max number of ADC samples possible on the wire

#ifndef __GCCXML__

  // partial constructor, used only as common part by the other constructors
      Wire(art::Ptr<raw::RawDigit> &rawdigit);
  
    public:
      
      // ROI constructor
      Wire(const RegionsOfInterest_t& sigROIlist,
           art::Ptr<raw::RawDigit> &rawdigit);
      Wire(RegionsOfInterest_t&& sigROIlist,
           art::Ptr<raw::RawDigit> &rawdigit);

      // Get Methods
      // zero-padded full length vector filled with ROIs
      std::vector<float>  Signal() const;

      const RegionsOfInterest_t& SignalROI()  const;
      size_t                     NSignal()    const;
      art::Ptr<raw::RawDigit>    RawDigit()   const;
      geo::View_t                View()       const;
      geo::SigType_t             SignalType() const;
      uint32_t                   Channel()    const;

#endif

    };
}

#ifndef __GCCXML__

inline const recob::Wire::RegionsOfInterest_t&
                                  recob::Wire::SignalROI()  const { return fSignalROI;          }
inline size_t                     recob::Wire::NSignal()    const { return fMaxSamples;         }
inline art::Ptr<raw::RawDigit>    recob::Wire::RawDigit()   const { return fRawDigit;           }
inline geo::View_t                recob::Wire::View()       const { return fView;               }
inline geo::SigType_t             recob::Wire::SignalType() const { return fSignalType;         }
inline uint32_t                   recob::Wire::Channel()    const { return fRawDigit->Channel();}

#endif

#endif // WIRE_H

////////////////////////////////////////////////////////////////////////
