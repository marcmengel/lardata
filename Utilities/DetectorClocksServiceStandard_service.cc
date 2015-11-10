#include "Utilities/DetectorClocksServiceStandard.h"
#include "TFile.h"
#include "art/Persistency/RootDB/SQLite3Wrapper.h"
#include "fhiclcpp/make_ParameterSet.h"

//-----------------------------------------------------------------------------------------
util::DetectorClocksServiceStandard::DetectorClocksServiceStandard(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg)
{
  
  reconfigure(pset);
  
  reg.sPreProcessEvent.watch (this, &DetectorClocksServiceStandard::preProcessEvent);
  reg.sPostOpenFile.watch    (this, &DetectorClocksServiceStandard::postOpenFile);
  reg.sPreBeginRun.watch     (this, &DetectorClocksServiceStandard::preBeginRun);
  
  fClocks.reset(new dataprov::DetectorClocksStandard());
  this->reconfigure(pset);
  
}

//------------------------------------------------------------------
void util::DetectorClocksServiceStandard::reconfigure(fhicl::ParameterSet const& pset)
//------------------------------------------------------------------
{
  fClocks->Configure(pset);

}

//------------------------------------------------------------
void util::DetectorClocksServiceStandard::preProcessEvent(const art::Event& evt)
//------------------------------------------------------------
{
  art::Handle<std::vector<raw::Trigger> > trig_handle;
  evt.getByLabel(fClocks->TrigModuleName(), trig_handle);

  std::vector<std::string> cfgNames = fClocks->ConfigNames();
  std::vector<double> cfgValues = fClocks->ConfigValues();
  
  if(!trig_handle.isValid()) {
    // Trigger simulation has not run yet!
    fClocks->SetTriggerTime(cfgValues.at(dataprov::kDefaultTrigTime),
			    cfgValues.at(dataprov::kDefaultBeamTime) );
    return;
  }

  if(trig_handle->size()>1)
    
    throw cet::exception("DetectorClocksServiceStandard::preProcessEvent")
      << "Found " << trig_handle->size() << " triggers (only 1 trigger/event supported)\n";
  
  const art::Ptr<raw::Trigger> trig_ptr(trig_handle,0);

  fClocks->SetTriggerTime(trig_ptr->TriggerTime(),
			  trig_ptr->BeamGateTime() );
  
  return;
}

//------------------------------------------------------
void util::DetectorClocksServiceStandard::preBeginRun(art::Run const& run)
//------------------------------------------------------
{

  if (run.id().run() < 0) return;
  
  fClocks->ApplyParams();

}


//---------------------------------------------------------------
void util::DetectorClocksServiceStandard::postOpenFile(const std::string& filename)
//---------------------------------------------------------------
{

  // Method inheriting from DetectorProperties
  
  if(!fClocks->InheritClockConfig()) return;

  // The only way to access art service metadata from the input file
  // is to open it as a separate TFile object.  Do that now.
  
  if(!filename.empty()) {
    
    TFile* file = TFile::Open(filename.c_str(), "READ");
    if(file != 0 && !file->IsZombie() && file->IsOpen()) {

      std::vector<std::string> cfgName = fClocks->ConfigNames();
      std::vector<double> cfgValue = fClocks->ConfigValues();
      
      // Open the sqlite datatabase.
      
      art::SQLite3Wrapper sqliteDB(file, "RootFileDB");
      
      // Loop over all stored ParameterSets.

      std::vector<size_t> config_count(dataprov::kInheritConfigTypeMax,0);
      std::vector<double> config_value(dataprov::kInheritConfigTypeMax,0);

      sqlite3_stmt * stmt = 0;
      sqlite3_prepare_v2(sqliteDB, "SELECT PSetBlob from ParameterSets;", -1, &stmt, NULL);

      while (sqlite3_step(stmt) == SQLITE_ROW) {

	fhicl::ParameterSet ps;
	fhicl::make_ParameterSet(reinterpret_cast<char const *>(sqlite3_column_text(stmt, 0)), ps);

	if(!fClocks->IsRightConfig(ps)) continue;

	for(size_t i=0; i<dataprov::kInheritConfigTypeMax; ++i) {

	  double value_from_file = ps.get<double>(cfgName.at(i).c_str());
	  
	  if(!(config_count.at(i)))

	    config_value.at(i) = value_from_file;

	  else if(config_value.at(i) != value_from_file)

	    throw cet::exception(__FUNCTION__) << Form("\033[95mFound historical value disagreement for %s ... %g != %g",
						       cfgName.at(i).c_str(),
						       config_value.at(i),
						       value_from_file)
					       << "\033[00m" << std::endl;
	  config_count.at(i) +=1;

	}
	
      }

      // Override parameters 

      for(size_t i=0; i<dataprov::kInheritConfigTypeMax; ++i)

	if(config_count.at(i) && cfgValue.at(i) != config_value.at(i)) {

	  std::cout  << Form("\033[93mOverriding configuration parameter %s ... %g (fcl) => %g (data file)\033[00m",
			     cfgName.at(i).c_str(),
			     cfgValue.at(i),
			     config_value.at(i))
		     << std::endl;
	  
	  fClocks->SetConfigValue(i,config_value.at(i));
	  
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
  fClocks->ApplyParams();

}

DEFINE_ART_SERVICE_INTERFACE_IMPL(util::DetectorClocksServiceStandard, util::DetectorClocksService)

