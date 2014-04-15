
#include "TimeService.h"

util::TimeService::TimeService(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg)
  : SimpleTimeService()
{
  reconfigure(pset);
}

void util::TimeService::reconfigure(fhicl::ParameterSet const& pset)
{
  fG4RefTime         = pset.get<double>( "G4RefTime"         );
  fFramePeriod       = pset.get<double>( "FramePeriod"       );
  fClockSpeedTPC     = pset.get<double>( "ClockSpeedTPC"     );
  fClockSpeedOptical = pset.get<double>( "ClockSpeedOptical" );
  fClockSpeedTrigger = pset.get<double>( "ClockSpeedTrigger" );
}

namespace util{

  DEFINE_ART_SERVICE(TimeService)

} // namespace util  

