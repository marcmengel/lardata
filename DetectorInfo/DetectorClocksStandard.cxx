#include "DetectorInfo/DetectorClocksStandard.h"

//-------------------------------------------------------------------------
detinfo::DetectorClocksStandard::DetectorClocksStandard()
  : fConfigName(detinfo::kInheritConfigTypeMax,""),
    fConfigValue(detinfo::kInheritConfigTypeMax,0),
    fTrigModuleName(""),
    fG4RefTime    (util::kDEFAULT_MC_CLOCK_T0),
    fFramePeriod  (util::kDEFAULT_FRAME_PERIOD),
    fTPCClock     (0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_TPC),
    fOpticalClock (0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_OPTICAL),
    fTriggerClock (0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_TRIGGER),
    fExternalClock(0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_EXTERNAL),
    fTriggerOffsetTPC     (util::kDEFAULT_TRIG_OFFSET_TPC),
    fTriggerTime  (0),
    fBeamGateTime (0)
{
  
  fConfigName.at(detinfo::kG4RefTime)         = "G4RefTime";
  fConfigName.at(detinfo::kTriggerOffsetTPC)  = "TriggerOffsetTPC";
  fConfigName.at(detinfo::kFramePeriod)       = "FramePeriod";
  fConfigName.at(detinfo::kClockSpeedTPC)     = "ClockSpeedTPC";
  fConfigName.at(detinfo::kClockSpeedOptical) = "ClockSpeedOptical";
  fConfigName.at(detinfo::kClockSpeedTrigger) = "ClockSpeedTrigger";
  fConfigName.at(detinfo::kClockSpeedExternal) = "ClockSpeedExternal";
  fConfigName.at(detinfo::kDefaultTrigTime)   = "DefaultTrigTime";
  fConfigName.at(detinfo::kDefaultBeamTime)   = "DefaultBeamTime";
  
  fInheritClockConfig = false;
}

//-------------------------------------------------------------------------
detinfo::DetectorClocksStandard::DetectorClocksStandard(fhicl::ParameterSet const& pset)
  : fConfigName(detinfo::kInheritConfigTypeMax,""),
    fConfigValue(detinfo::kInheritConfigTypeMax,0),
    fTrigModuleName(""),
    fG4RefTime    (util::kDEFAULT_MC_CLOCK_T0),
    fFramePeriod  (util::kDEFAULT_FRAME_PERIOD),
    fTPCClock     (0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_TPC),
    fOpticalClock (0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_OPTICAL),
    fTriggerClock (0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_TRIGGER),
    fExternalClock(0,util::kDEFAULT_FRAME_PERIOD,util::kDEFAULT_FREQUENCY_EXTERNAL),
    fTriggerOffsetTPC     (util::kDEFAULT_TRIG_OFFSET_TPC),
    fTriggerTime  (0),
    fBeamGateTime (0)
{
  
  fConfigName.at(detinfo::kG4RefTime)         = "G4RefTime";
  fConfigName.at(detinfo::kTriggerOffsetTPC)  = "TriggerOffsetTPC";
  fConfigName.at(detinfo::kFramePeriod)       = "FramePeriod";
  fConfigName.at(detinfo::kClockSpeedTPC)     = "ClockSpeedTPC";
  fConfigName.at(detinfo::kClockSpeedOptical) = "ClockSpeedOptical";
  fConfigName.at(detinfo::kClockSpeedTrigger) = "ClockSpeedTrigger";
  fConfigName.at(detinfo::kClockSpeedExternal) = "ClockSpeedExternal";
  fConfigName.at(detinfo::kDefaultTrigTime)   = "DefaultTrigTime";
  fConfigName.at(detinfo::kDefaultBeamTime)   = "DefaultBeamTime";
  
  fInheritClockConfig = false;
  Configure(pset);

}


//------------------------------------------------------------------
bool detinfo::DetectorClocksStandard::Update(uint64_t ts)
{
  return true;
}

//------------------------------------------------------------------
bool detinfo::DetectorClocksStandard::Configure(fhicl::ParameterSet const& pset)
{

  // Read fcl parameters
  fTrigModuleName                     = pset.get< std::string >( "TrigModuleName" );
  fInheritClockConfig                 = pset.get< bool >( "InheritClockConfig" );
  fConfigValue.at(kG4RefTime)         = pset.get< double >( fConfigName.at(kG4RefTime).c_str() );
  fConfigValue.at(kFramePeriod)       = pset.get< double >( fConfigName.at(kFramePeriod).c_str() );
  fConfigValue.at(kTriggerOffsetTPC)  = pset.get< double >( fConfigName.at(kTriggerOffsetTPC).c_str());
  fConfigValue.at(kClockSpeedTPC)     = pset.get< double >( fConfigName.at(kClockSpeedTPC).c_str() );
  fConfigValue.at(kClockSpeedOptical) = pset.get< double >( fConfigName.at(kClockSpeedOptical).c_str());
  fConfigValue.at(kClockSpeedTrigger) = pset.get< double >( fConfigName.at(kClockSpeedTrigger).c_str() );
  fConfigValue.at(kClockSpeedExternal)= pset.get< double >( fConfigName.at(kClockSpeedExternal).c_str());
 fConfigValue.at(kDefaultTrigTime)    = pset.get< double >( fConfigName.at(kDefaultTrigTime).c_str() );
  fConfigValue.at(kDefaultBeamTime)   = pset.get< double >( fConfigName.at(kDefaultBeamTime).c_str() );

  // Save fcl parameters in a container to check for inheritance
  fBeamGateTime = fTriggerTime = 0;

  ApplyParams();

  return true;
}

//-----------------------------------
void detinfo::DetectorClocksStandard::ApplyParams()
//-----------------------------------
{

  fG4RefTime   = fConfigValue.at(kG4RefTime);
  fFramePeriod = fConfigValue.at(kFramePeriod);
  fTriggerOffsetTPC = fConfigValue.at(kTriggerOffsetTPC);

  fTPCClock     = ::util::ElecClock( fTriggerTime, fFramePeriod, fConfigValue.at( kClockSpeedTPC     ) );
  fOpticalClock = ::util::ElecClock( fTriggerTime, fFramePeriod, fConfigValue.at( kClockSpeedOptical ) );
  fTriggerClock = ::util::ElecClock( fTriggerTime, fFramePeriod, fConfigValue.at( kClockSpeedTrigger ) );

}

//------------------------------------------------------------------------
bool detinfo::DetectorClocksStandard::IsRightConfig(const fhicl::ParameterSet& ps) const
//------------------------------------------------------------------------
{
  std::string s;
  double d;

  bool result = !ps.get_if_present("module_label", s);
  for(size_t i=0; result && i<kInheritConfigTypeMax; ++i)

    result = result && ps.get_if_present(fConfigName.at(i).c_str(),d);

  return result;
}

//-----------------------------------------
void detinfo::DetectorClocksStandard::debugReport() const
//-----------------------------------------
{
  std::cout << "fConfigValues contents: "<<std::endl;
  for(size_t i=0;i<kInheritConfigTypeMax; ++i)

    std::cout<<"    "<<fConfigName.at(i).c_str()<<" ... "<<fConfigValue.at(i)<<std::endl;

  std::cout<<std::endl;
  
  std::cout 
    << "Trigger  time @ " << fTriggerTime       << std::endl
    << "BeamGate time @ " << fBeamGateTime      << std::endl
    << "TrigOffsetTPC @ " << TriggerOffsetTPC() << std::endl
    << "G4RefTime     @ " << fG4RefTime         << std::endl
    << "TPC     Freq. @ " << fTPCClock.Frequency() << std::endl
    << "Optical Freq. @ " << fOpticalClock.Frequency() << std::endl
    << "Trigger Freq. @ " << fTriggerClock.Frequency() << std::endl
    << "External Freq. @ " << fExternalClock.Frequency() << std::endl
    << "TPC start tick [tdc]             : " << TPCTick2TDC(0) <<std::endl
    << "TPC start tick from trigger [us] : " << TPCTick2TrigTime(0) <<std::endl
    << "TPC start tick from beam    [us] : " << TPCTick2BeamTime(0) <<std::endl
    << "TPC tdc=0 in tick     : " << TPCTDC2Tick(0) << std::endl
    << "TPC G4 time 0 in tick : " << TPCG4Time2Tick(0) << std::endl
    << std::endl;
  
}

