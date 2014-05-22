
#include "TimeService.h"

//-----------------------------------------------------------------------------------------
util::TimeService::TimeService(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg)
  : SimpleTimeService(),
    fConfigName(kInheritConfigTypeMax,""),
    fConfigValue(kInheritConfigTypeMax,0),
    fTrigModuleName("")
//----------------------------------------------------------------------------------------
{

  fConfigName.at(kG4RefTime)         = "G4RefTime";
  fConfigName.at(kTriggerOffsetTPC)  = "TriggerOffsetTPC";
  fConfigName.at(kFramePeriod)       = "FramePeriod";
  fConfigName.at(kClockSpeedTPC)     = "ClockSpeedTPC";
  fConfigName.at(kClockSpeedOptical) = "ClockSpeedOptical";
  fConfigName.at(kClockSpeedTrigger) = "ClockSpeedTrigger";

  fInheritClockConfig = false;
  fAlreadyReadFromDB  = false;
  reconfigure(pset);

  reg.sPreProcessEvent.watch (this, &TimeService::preProcessEvent);
  reg.sPostOpenFile.watch    (this, &TimeService::postOpenFile );
  reg.sPreBeginRun.watch     (this, &TimeService::preBeginRun  );

}

//------------------------------------------------------------------
void util::TimeService::reconfigure(fhicl::ParameterSet const& pset)
//------------------------------------------------------------------
{
  // Read fcl parameters
  fTrigModuleName                     = pset.get< std::string >( "TrigModuleName"                           );
  fInheritClockConfig                 = pset.get< bool        >( "InheritClockConfig"                       );
  fConfigValue.at(kG4RefTime)         = pset.get< double      >( fConfigName.at(kG4RefTime).c_str()         );
  fConfigValue.at(kFramePeriod)       = pset.get< double      >( fConfigName.at(kFramePeriod).c_str()       );
  fConfigValue.at(kTriggerOffsetTPC)  = pset.get< double      >( fConfigName.at(kTriggerOffsetTPC).c_str()  );
  fConfigValue.at(kClockSpeedTPC)     = pset.get< double      >( fConfigName.at(kClockSpeedTPC).c_str()     );
  fConfigValue.at(kClockSpeedOptical) = pset.get< double      >( fConfigName.at(kClockSpeedOptical).c_str() );
  fConfigValue.at(kClockSpeedTrigger) = pset.get< double      >( fConfigName.at(kClockSpeedTrigger).c_str() );

  // Save fcl parameters in a container to check for inheritance
  fBeamGateTime = fTriggerTime = 0;

  ApplyParams();
}

//-----------------------------------
void util::TimeService::ApplyParams()
//-----------------------------------
{
  fG4RefTime   = fConfigValue.at(kG4RefTime);
  fFramePeriod = fConfigValue.at(kFramePeriod);
  fTriggerOffsetTPC = fConfigValue.at(kTriggerOffsetTPC);
  
  fTPCClock     = ::util::ElecClock( fTriggerTime, fFramePeriod, fConfigValue.at( kClockSpeedTPC     ) );
  fOpticalClock = ::util::ElecClock( fTriggerTime, fFramePeriod, fConfigValue.at( kClockSpeedOptical ) );
  fTriggerClock = ::util::ElecClock( fTriggerTime, fFramePeriod, fConfigValue.at( kClockSpeedTrigger ) );
}

//------------------------------------------------------------
void util::TimeService::preProcessEvent(const art::Event& evt)
//------------------------------------------------------------
{

  art::Handle<std::vector<raw::Trigger> > trig_handle;
  evt.getByLabel(fTrigModuleName, trig_handle);

  if(!trig_handle.isValid()) {
    // Trigger simulation has not run yet!
    SetTriggerTime(0,0);
    return;
  }

  if(trig_handle->size()>1)

    throw cet::exception(Form("Found %zu triggers (only 1 trigger/event supported)",trig_handle->size()));

  const art::Ptr<raw::Trigger> trig_ptr(trig_handle,0);

  SetTriggerTime(trig_ptr->TriggerTime(),
		 trig_ptr->BeamGateTime() );

  return;
}

//------------------------------------------------------
void util::TimeService::preBeginRun(art::Run const& run)
//------------------------------------------------------
{
  int nrun = run.id().run();
  art::ServiceHandle<util::DatabaseUtil> DButil;
  if (nrun != 0){
    
    double inpvalue = 0.;
    
    //get T0 for a given run. If it doesn't work return to default value.
    if(DButil->GetTriggerOffsetFromDB(nrun,inpvalue)!=-1)
      fConfigValue.at(kTriggerOffsetTPC) = inpvalue;
    
  }
  else
    std::cerr<< "run number == 0, not extracting info from DB\n" ;

  ApplyParams();
  fAlreadyReadFromDB=true;
}

//--------------------------------------------
void util::TimeService::CheckDBStatus() const
//--------------------------------------------
{
  bool fToughErrorTreatment= art::ServiceHandle<util::DatabaseUtil>()->ToughErrorTreatment();
  bool fShouldConnect =  art::ServiceHandle<util::DatabaseUtil>()->ShouldConnect();
  //Have not read from DB, should read and requested tough treatment
  if(!fAlreadyReadFromDB && fToughErrorTreatment && fShouldConnect )
    std::cerr << " Extracting values from DetectorProperties before they "
	      << " have been read in from database. \n "
	      << "Set ToughErrorTreatment or ShouldConnect "
	      << " to false in databaseutil.fcl if you want "
	      << " to avoid this. \n";
  //Have not read from DB, should read and requested soft treatment
  else if(!fAlreadyReadFromDB && !fToughErrorTreatment && fShouldConnect )
    std::cerr << "!!! Extracting values from DetectorProperties before they "
	      << " have been read in from the database. \n "
	      << " You may not be using the correct values of "
	      << " T0!"
	      << " You should not be initializing"
	      << " Database originating values in BeginJob()s or constructors."
	      << " You have been warned !!! \n ";
  
  //In other cases, either already read from DB, or should not connect so it doesn't matter
}

//---------------------------------------------------------------
void util::TimeService::postOpenFile(const std::string& filename)
//---------------------------------------------------------------
{
  // Method inheriting from DetectorProperties

  if(!fInheritClockConfig) return;

  // The only way to access art service metadata from the input file
  // is to open it as a separate TFile object.  Do that now.
  
  if(!filename.empty()) {
    
    TFile* file = new TFile(filename.c_str(), "READ");
    if(file != 0 && !file->IsZombie() && file->IsOpen()) {
      
      // Open the sqlite datatabase.
      
      art::SQLite3Wrapper sqliteDB(file, "RootFileDB");
      
      // Loop over all stored ParameterSets.

      std::vector<size_t> config_count(kInheritConfigTypeMax,0);
      std::vector<double> config_value(kInheritConfigTypeMax,0);

      sqlite3_stmt * stmt = 0;
      sqlite3_prepare_v2(sqliteDB, "SELECT PSetBlob from ParameterSets;", -1, &stmt, NULL);

      while (sqlite3_step(stmt) == SQLITE_ROW) {

	fhicl::ParameterSet ps;
	fhicl::make_ParameterSet(reinterpret_cast<char const *>(sqlite3_column_text(stmt, 0)), ps);

	if(!IsRightConfig(ps)) continue;

	for(size_t i=0; i<kInheritConfigTypeMax; ++i) {

	  double value_from_file = ps.get<double>(fConfigName.at(i).c_str());
	  
	  if(!(config_count.at(i)))

	    config_value.at(i) = value_from_file;

	  else if(config_value.at(i) != value_from_file)

	    throw cet::exception(__FUNCTION__) << Form("Found historical value disagreement for %s ... %g != %g",
						       fConfigName.at(i).c_str(),
						       config_value.at(i),
						       value_from_file);
	  config_count.at(i) +=1;

	}
	
      }

      // Override parameters 

      for(size_t i=0; i<kInheritConfigTypeMax; ++i)

	if(config_count.at(i) && fConfigValue.at(i) != config_value.at(i)) {

	  std::cout  << Form("Overriding configuration parameter %s ... %g (fcl) => %g (data file)",
			     fConfigName.at(i).c_str(),
			     fConfigValue.at(i),
			     config_value.at(i));
	  
	}
    }
    
    // Close file.
    if(file != 0) {
      if(file->IsOpen())
	file->Close();
      delete file;
    }
  }

  // Reset parameters
  ApplyParams();

}

//------------------------------------------------------------------------
bool util::TimeService::IsRightConfig(const fhicl::ParameterSet& ps) const
//------------------------------------------------------------------------
{
  std::string s;
  double d;

  bool result = !ps.get_if_present("module_label", s);
  for(size_t i=0; result && i<kInheritConfigTypeMax; ++i)

    result = result && ps.get_if_present(fConfigName.at(i).c_str(),d);

  return result;
}

namespace util{

  DEFINE_ART_SERVICE(TimeService)

} // namespace util  

