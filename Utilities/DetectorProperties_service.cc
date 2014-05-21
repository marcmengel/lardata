////////////////////////////////////////////////////////////////////////
//
//  \file DetectorProperties_service.cc
//
////////////////////////////////////////////////////////////////////////
// Framework includes

// LArSoft includes
#include "Utilities/DetectorProperties.h"
#include "Utilities/LArProperties.h"
#include "Geometry/Geometry.h"
#include "Geometry/CryostatGeo.h"
#include "Geometry/TPCGeo.h"
#include "Geometry/PlaneGeo.h"
#include "Utilities/DatabaseUtil.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// Art includes
#include "art/Persistency/RootDB/SQLite3Wrapper.h"
#include "fhiclcpp/make_ParameterSet.h"

namespace util{

  //--------------------------------------------------------------------
  DetectorProperties::DetectorProperties(fhicl::ParameterSet const& pset, 
					 art::ActivityRegistry &reg) 
    : fNumberTimeSamples(pset.get< unsigned int >("NumberTimeSamples"))
  {
    this->reconfigure(pset);

    // Register for callbacks.

    reg.sPostOpenFile.watch(this, &DetectorProperties::postOpenFile);
    reg.sPreBeginRun.watch(this, &DetectorProperties::preBeginRun);
  }

  //--------------------------------------------------------------------
  DetectorProperties::~DetectorProperties() 
  {
    
  }
  


  //--------------------------------------------------------------------
  void DetectorProperties::reconfigure(fhicl::ParameterSet const& p)
  {
    //fSamplingRate             = p.get< double        >("SamplingRate"     );
    fTriggerOffset     	      = p.get< int    	     >("TriggerOffset"    );
    fElectronsToADC    	      = p.get< double 	     >("ElectronsToADC"   );
    fNumberTimeSamples 	      = p.get< unsigned int >("NumberTimeSamples");
    fReadOutWindowSize 	      = p.get< unsigned int >("ReadOutWindowSize");
    fTimeOffsetU       	      = p.get< double 	     >("TimeOffsetU"      );
    fTimeOffsetV       	      = p.get< double 	     >("TimeOffsetV"      );
    fTimeOffsetZ       	      = p.get< double 	     >("TimeOffsetZ"      );
    fInheritTriggerOffset     = p.get<bool           >("InheritTriggerOffset",     false);
    fInheritNumberTimeSamples = p.get<bool           >("InheritNumberTimeSamples", false);
    fXTicksParamsLoaded = false;

    art::ServiceHandle<util::TimeService> ts;
    fTPCClock = ts->TPCClock();
    // Save the parameter set.
    fPS = p;

    return;
  }


  
  //----------------------------------------------
void DetectorProperties::preBeginRun(art::Run const& run)
{
    int nrun = run.id().run();
    art::ServiceHandle<util::DatabaseUtil> DButil;
    if (nrun != 0){

    double inpvalue = 0.;

    //get T0 for a given run. If it doesn't work return to default value.
    if(DButil->GetTriggerOffsetFromDB(nrun,inpvalue)!=-1)
      fTriggerOffset=inpvalue;
    
    }
  else
    mf::LogWarning("DetectorProperties") << "run number == 0, not extracting info from DB\n" ;

  fAlreadyReadFromDB=true;
}

  //---------------------------------------------------------------------------------
void DetectorProperties::checkDBstatus() const
{
  bool fToughErrorTreatment= art::ServiceHandle<util::DatabaseUtil>()->ToughErrorTreatment();
  bool fShouldConnect =  art::ServiceHandle<util::DatabaseUtil>()->ShouldConnect();
  //Have not read from DB, should read and requested tough treatment
    if(!fAlreadyReadFromDB && fToughErrorTreatment && fShouldConnect )
      throw cet::exception("DetectorProperties") << " Extracting values from DetectorProperties before they "
              << " have been read in from database. \n "
              << "Set ToughErrorTreatment or ShouldConnect "
              << " to false in databaseutil.fcl if you want "
              << " to avoid this. \n";
   //Have not read from DB, should read and requested soft treatment
    else if(!fAlreadyReadFromDB && !fToughErrorTreatment && fShouldConnect )
      mf::LogWarning("DetectorProperties") <<  "!!! Extracting values from DetectorProperties before they "
              << " have been read in from the database. \n "
              << " You may not be using the correct values of "
              << " T0!"
              << " You should not be initializing"
              << " Database originating values in BeginJob()s or constructors."
              << " You have been warned !!! \n ";

    //In other cases, either already read from DB, or should not connect so it doesn't matter
}

//------------------------------------------------------------------------------------//
int  DetectorProperties::TriggerOffset()     const 
{
  this->checkDBstatus();
  return fTriggerOffset; 
}


  //--------------------------------------------------------------------
  //  x<--> ticks conversion methods 
  //
  //  Ben Jones April 2012, 
  //  based on code by Herb Greenlee in SpacePointService
  //  




  //--------------------------------------------------------------------
  // Take an X coordinate, and convert to a number of ticks, the
  // charge deposit occured at t=0
 
  double DetectorProperties::ConvertXToTicks(double X, int p, int t, int c)
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return (X / fXTicksCoefficient +  fXTicksOffsets.at(c).at(t).at(p) );
  }



  //-------------------------------------------------------------------
  // Take a cooridnate in ticks, and convert to an x position
  // assuming event deposit occured at t=0
 
  double  DetectorProperties::ConvertTicksToX(double ticks, int p, int t, int c)
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return (ticks - fXTicksOffsets.at(c).at(t).at(p) ) * fXTicksCoefficient;  
  }


  //--------------------------------------------------------------------
  // Recalculte x<-->ticks conversion parameters from detector constants

  void DetectorProperties::CalculateXTicksParams()
  {
    art::ServiceHandle<util::LArProperties>      lrp;
    art::ServiceHandle<geo::Geometry>            geo;

    double samplingRate   = SamplingRate();
    double efield         = lrp->Efield();
    double temperature    = lrp->Temperature();
    double driftVelocity  = lrp->DriftVelocity(efield, temperature);
    
    fXTicksCoefficient    = 0.001 * driftVelocity * samplingRate;

    double triggerOffset  = TriggerOffset();

    fXTicksOffsets.clear();
    fXTicksOffsets.resize(geo->Ncryostats());

    for(size_t cstat = 0; cstat < geo->Ncryostats(); ++cstat){
      fXTicksOffsets[cstat].resize(geo->Cryostat(cstat).NTPC());
      
      for(size_t tpc = 0; tpc < geo->Cryostat(cstat).NTPC(); ++tpc) {
	const geo::TPCGeo& tpcgeom = geo->Cryostat(cstat).TPC(tpc);
	
	int nplane = tpcgeom.Nplanes();
	fXTicksOffsets[cstat][tpc].resize(nplane, 0.);
	for(int plane = 0; plane < nplane; ++plane) {
	  const geo::PlaneGeo& pgeom = tpcgeom.Plane(plane);
	  
	  
	  // Get field in gap between planes
	  double efieldgap[3];
	  double driftVelocitygap[3];
	  double fXTicksCoefficientgap[3];
	  for (int igap = 0; igap<3; ++igap){
	    efieldgap[igap] = lrp->Efield(igap);
	    driftVelocitygap[igap] = lrp->DriftVelocity(efieldgap[igap], temperature);
	    fXTicksCoefficientgap[igap] = 0.001 * driftVelocitygap[igap] * samplingRate;
	  }
	  
	  // Calculate geometric time offset.
	  // only works if xyz[0]<=0
	  const double* xyz = tpcgeom.PlaneLocation(0);
	  
	  fXTicksOffsets[cstat][tpc][plane] = -xyz[0]/fXTicksCoefficient + triggerOffset;

	  if (nplane==3){
	    /*
	 |    ---------- plane = 2 (collection)
	 |                      Coeff[2]
	 |    ---------- plane = 1 (2nd induction)
	 |                      Coeff[1]
	 |    ---------- plane = 0 (1st induction) x = xyz[0]
	 |                      Coeff[0]
	 |    ---------- x = 0
	 V     For plane = 0, t offset is -xyz[0]/Coeff[0]
	 x   */
	    for (int ip = 0; ip < plane; ++ip){
	      fXTicksOffsets[cstat][tpc][plane] += tpcgeom.PlanePitch(ip,ip+1)/fXTicksCoefficientgap[ip+1];
	    }
	  }	  
	  else if (nplane==2){ ///< special case for ArgoNeuT
	    /*
	 |    ---------- plane = 1 (collection)
	 |                      Coeff[2]
	 |    ---------- plane = 0 (2nd induction) x = xyz[0]
	 |    ---------- x = 0, Coeff[1]
	 V    ---------- first induction plane
	 x                      Coeff[0]
For plane = 0, t offset is pitch/Coeff[1] - (pitch+xyz[0])/Coeff[0]
                         = -xyz[0]/Coeff[0] - pitch*(1/Coeff[0]-1/Coeff[1])
	    */
	    for (int ip = 0; ip < plane; ++ip){
	      fXTicksOffsets[cstat][tpc][plane] += tpcgeom.PlanePitch(ip,ip+1)/fXTicksCoefficientgap[ip+2];
	    }
	    fXTicksOffsets[cstat][tpc][plane] -= tpcgeom.PlanePitch()*(1/fXTicksCoefficient-1/fXTicksCoefficientgap[1]);
	  }
	  
	  // Add view dependent offset
	  geo::View_t view = pgeom.View();
	  if(view == geo::kU)
	    fXTicksOffsets[cstat][tpc][plane] += fTimeOffsetU;
	  else if(view == geo::kV)
	    fXTicksOffsets[cstat][tpc][plane] += fTimeOffsetV;
	  else if(view == geo::kZ)
	    fXTicksOffsets[cstat][tpc][plane] += fTimeOffsetZ;
	  else
	    throw cet::exception("DetectorProperties") << "Bad view = "
						       << view << "\n" ;
	}	
      }
    }

    fXTicksParamsLoaded=true;
  }


  //--------------------------------------------------------------------
  // Get scale factor for x<-->ticks

  double DetectorProperties::GetXTicksCoefficient()
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return fXTicksCoefficient;
  }



  //--------------------------------------------------------------------
  //  Get offset for x<-->ticks

  double DetectorProperties::GetXTicksOffset(int p, int t, int c) 
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return fXTicksOffsets.at(c).at(t).at(p);	
  }

  //--------------------------------------------------------------------
  //  Callback called after input file is opened.

  void DetectorProperties::postOpenFile(const std::string& filename)
  {
    // Use this method to figure out whether to inherit configuration
    // parameters from previous jobs.
    //
    // There is no way currently to correlate parameter sets saved in
    // sqlite RootFileDB with process history (from MetaData tree).
    // Therefore, we use the approach of scanning every historical
    // parameter set in RootFileDB, and finding all parameter sets
    // that appear to be DetectorProperties configurations.  If all
    // historical parameter sets are in agreement about the value of
    // an inherited parameter, then we accept the historical value,
    // print a message, and override the configuration parameter.  In
    // cases where the historical configurations are not in agreement
    // about the value of an inherited parameter, we ignore any
    // historical parameter values that are the same as the current
    // configured value of the parameter (that is, we resolve the
    // conflict in favor of parameters values that are different than
    // the current configuration).  If two or more historical values
    // differ from the current configuration, throw an exception.
    // Note that it is possible to give precendence to the current
    // configuration by disabling inheritance for that configuration
    // parameter.

    // Don't do anything if no parameters are supposed to be inherited.

    if(!fInheritTriggerOffset && !fInheritNumberTimeSamples)
      return;

    // The only way to access art service metadata from the input file
    // is to open it as a separate TFile object.  Do that now.

    if(filename.size() != 0) {

      TFile* file = new TFile(filename.c_str(), "READ");
      if(file != 0 && !file->IsZombie() && file->IsOpen()) {

	// Open the sqlite datatabase.

	art::SQLite3Wrapper sqliteDB(file, "RootFileDB");

	// Loop over all stored ParameterSets.

	int iTriggerOffset = 0;               // Combined value of TriggerOffset.
	unsigned int nTriggerOffset = 0;      // Number of TriggerOffset parameters seen.
	unsigned int iNumberTimeSamples = 0;  // Combined value of NumberTimeSamples.
	unsigned int nNumberTimeSamples = 0;  // Number of NumberTimeSamples parameters seen.

	sqlite3_stmt * stmt = 0;
	sqlite3_prepare_v2(sqliteDB, "SELECT PSetBlob from ParameterSets;", -1, &stmt, NULL);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
	  fhicl::ParameterSet ps;
	  fhicl::make_ParameterSet(reinterpret_cast<char const *>(sqlite3_column_text(stmt, 0)), ps);
	  // Is this a DetectorProperties parameter set?

	  bool psok = isDetectorProperties(ps);
	  if(psok) {

	    // Check TriggerOffset

	    if(fInheritTriggerOffset) {
	      int newTriggerOffset = ps.get<int>("TriggerOffset");

	      // Ignore parameter values that match the current configuration.

	      if(newTriggerOffset != fPS.get<int>("TriggerOffset")) {
		if(nTriggerOffset == 0)
		  iTriggerOffset = newTriggerOffset;
		else if(newTriggerOffset != iTriggerOffset) {
		  throw cet::exception("DetectorProperties")
		    << "Historical values of TriggerOffset do not agree: "
		    << iTriggerOffset << " " << newTriggerOffset << "\n" ;
		}
		++nTriggerOffset;
	      }
	    }

	    // Check NumberTimeSamples

	    if(fInheritNumberTimeSamples) {
	      unsigned int newNumberTimeSamples = ps.get<unsigned int>("NumberTimeSamples");

	      // Ignore parameter values that match the current configuration.

	      if(newNumberTimeSamples != fPS.get<unsigned int>("NumberTimeSamples")) {
		if(nNumberTimeSamples == 0)
		  iNumberTimeSamples = newNumberTimeSamples;
		else if(newNumberTimeSamples != iNumberTimeSamples) {
		  throw cet::exception("DetectorProperties")
		    << "Historical values of NumberTimeSamples do not agree: "
		    << iNumberTimeSamples << " " << newNumberTimeSamples << "\n" ;
		}
		++nNumberTimeSamples;
	      }
	    }
	  }
	}

	// Done looping over parameter sets.
	// Now decide which parameters we will actually override.

	if(fInheritTriggerOffset && 
	   nTriggerOffset != 0 &&
	   iTriggerOffset != fTriggerOffset) {
	  mf::LogInfo("DetectorProperties")
	    << "Overriding configuration parameter TriggerOffset using historical value.\n"
	    << "  Configured value:        " << fTriggerOffset << "\n"
	    << "  Historical (used) value: " << iTriggerOffset << "\n";
	  fTriggerOffset = iTriggerOffset;
	}
	
	if(fInheritNumberTimeSamples && 
	   nNumberTimeSamples != 0 && 
	   iNumberTimeSamples != fNumberTimeSamples) {
	  mf::LogInfo("DetectorProperties")
	    << "Overriding configuration parameter NumberTimeSamples using historical value.\n"
	    << "  Configured value:        " << fNumberTimeSamples << "\n"
	    << "  Historical (used) value: " << iNumberTimeSamples << "\n";
	  fNumberTimeSamples = iNumberTimeSamples;
	}
      }

      // Close file.

      if(file != 0) {
	if(file->IsOpen())
	  file->Close();
	delete file;
      }
    }
  }

  //--------------------------------------------------------------------
  //  Determine whether a parameter set is a DetectorProperties configuration.

  bool DetectorProperties::isDetectorProperties(const fhicl::ParameterSet& ps)
  {
    // This method uses heuristics to determine whether the parameter
    // set passed as argument is a DetectorProperties configuration
    // parameter set.

    std::string s;
    double d;
    int i;
    unsigned int u;
    
    bool result = !ps.get_if_present("module_label", s);
    result = result && ps.get_if_present("TriggerOffset", i);
    result = result && ps.get_if_present("SamplingRate", d);
    result = result && ps.get_if_present("NumberTimeSamples", u);
    result = result && ps.get_if_present("ReadOutWindowSize", u);

    return result;
  }

} // namespace

namespace util{
 
  DEFINE_ART_SERVICE(DetectorProperties)

} // namespace util
