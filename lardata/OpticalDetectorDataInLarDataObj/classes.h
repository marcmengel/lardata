// The headers required to built the ART dictionary for this
// package. Also see classes_def.xml.

#include "canvas/Persistency/Common/Wrapper.h"
#include "lardataobj/OpticalDetectorData/OpticalTypes.h"
#include "lardataobj/OpticalDetectorData/ChannelData.h"
#include "lardataobj/OpticalDetectorData/ChannelDataGroup.h"
#include "lardataobj/OpticalDetectorData/FIFOChannel.h"
#include "lardataobj/OpticalDetectorData/PMTTrigger.h"
#include "lardataobj/OpticalDetectorData/OpticalRawDigit.h"

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

template class art::Wrapper< std::vector<optdata::ADC_Count_t>      >;
template class art::Wrapper< std::vector< std::pair< optdata::TimeSlice_t, optdata::TimeSlice_t > > >;
template class art::Wrapper< std::vector<optdata::ChannelData>      >;
template class art::Wrapper< optdata::ChannelDataGroup              >;
template class art::Wrapper< std::vector<optdata::ChannelDataGroup> >;
template class art::Wrapper< std::vector<optdata::FIFOChannel>      >;
template class art::Wrapper< std::vector<optdata::PMTTrigger>       >;
template class art::Wrapper< std::vector<optdata::OpticalRawDigit>  >;
