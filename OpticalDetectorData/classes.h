// The headers required to built the ART dictionary for this
// package. Also see classes_def.xml.

#include "art/Persistency/Common/Wrapper.h"
#include "OpticalDetectorData/OpticalTypes.h"
#include "OpticalDetectorData/ChannelData.h"
#include "OpticalDetectorData/ChannelDataGroup.h"
#include "OpticalDetectorData/FIFOChannel.h"
#include "OpticalDetectorData/PMTTrigger.h"

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

template class art::Wrapper< std::vector<optdata::ADC_Count_t>      >;
template class art::Wrapper< std::vector< std::pair< optdata::TimeSlice_t, optdata::TimeSlice_t > > >;
template class art::Wrapper< std::vector<optdata::ChannelData>      >;
template class art::Wrapper< std::vector<optdata::ChannelDataGroup> >;
template class art::Wrapper< std::vector<optdata::FIFOChannel>      >;
template class art::Wrapper< std::vector<optdata::PMTTrigger>       >;
