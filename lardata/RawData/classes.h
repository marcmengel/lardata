//
// Build a dictionary.
//
// $Id: classes.h,v 1.8 2010/04/12 18:12:28  Exp $
// $Author:  $
// $Date: 2010/04/12 18:12:28 $
// 
// Original author Rob Kutschke, modified by klg
//
// Notes:
// 1) The system is not able to deal with
//    art::Wrapper<std::vector<std::string> >;
//    The problem is somewhere inside root's reflex mechanism
//    and Philippe Canal says that it is ( as of March 2010) a
//    known problem.  He also says that they do not have any
//    plans to fix it soon.  We can always work around it 
//    by putting the string inside another object.

#include "art/Persistency/Common/Wrapper.h"

#include "lardata/RawData/DAQHeader.h"
#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/OpDetPulse.h"
#include "lardata/RawData/AuxDetDigit.h"
#include "lardata/RawData/BeamInfo.h"
#include "lardata/RawData/ExternalTrigger.h"
#include "lardata/RawData/TriggerData.h"
#include "lardata/RawData/OpDetWaveform.h"

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//
template class std::vector<raw::DAQHeader>;
template class std::vector<raw::BeamInfo>;
template class std::vector<raw::RawDigit>;
template class std::vector<raw::OpDetPulse>;
template class std::vector<raw::OpDetWaveform>;
template class std::vector<raw::AuxDetDigit>;
template class std::vector<raw::ExternalTrigger>;
template class std::vector<raw::Trigger>;

template class std::pair<std::string,std::vector<double> >;
template class std::map<std::string,std::vector<double> >;

template class art::Wrapper< raw::DAQHeader                >;
template class art::Wrapper< raw::OpDetPulse               >;
template class art::Wrapper< raw::AuxDetDigit              >;
template class art::Wrapper< raw::BeamInfo                 >;
template class art::Wrapper< raw::ExternalTrigger          >;
template class art::Wrapper< raw::Trigger                  >;
template class art::Wrapper< raw::OpDetWaveform            >;

template class art::Wrapper< std::vector<raw::DAQHeader>   >;
template class art::Wrapper< std::vector<raw::BeamInfo>    >;
template class art::Wrapper< std::vector<raw::RawDigit>    >;
template class art::Wrapper< std::vector<raw::OpDetPulse>  >;
template class art::Wrapper< std::vector<raw::AuxDetDigit> >;
template class art::Wrapper< std::vector<raw::ExternalTrigger> >;
template class art::Wrapper< std::vector<raw::Trigger>     >;
template class art::Wrapper< std::vector<raw::OpDetWaveform> >;
