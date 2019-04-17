////////////////////////////////////////////////////////////////////////
//
//  \file DetectorPropertiesServiceArgoNeuT_service.cc
//
////////////////////////////////////////////////////////////////////////
// Framework includes

// LArSoft includes
#include "lardata/Utilities/DetectorPropertiesServiceArgoNeuT.h"
#include "lardata/DetectorInfoServices/LArPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/CryostatGeo.h"
#include "larcorealg/Geometry/TPCGeo.h"
#include "larcorealg/Geometry/PlaneGeo.h"
#include "lardata/Utilities/DatabaseUtil.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// Art includes
#include "art_root_io/RootDB/SQLite3Wrapper.h"
#include "canvas/Utilities/Exception.h"
#include "fhiclcpp/make_ParameterSet.h"

namespace util{

  //--------------------------------------------------------------------
  DetectorPropertiesServiceArgoNeuT::DetectorPropertiesServiceArgoNeuT(fhicl::ParameterSet const& pset,
					 art::ActivityRegistry &reg)
    : fNumberTimeSamples(pset.get< unsigned int >("NumberTimeSamples"))
  {
    fLP = dynamic_cast<util::LArPropertiesServiceArgoNeuT const*>
      (lar::providerFrom<detinfo::LArPropertiesService>());
    if (!fLP) {
      // this legacy service works only coupled to the corresponding
      // LArProperties legacy service:
      throw art::Exception(art::errors::Configuration)
        << "DetectorPropertiesServiceArgoNeuT service requires"
        " LArPropertiesServiceArgoNeuT";
    }

    this->reconfigure(pset);

    // Register for callbacks.

    reg.sPostOpenFile.watch    (this, &DetectorPropertiesServiceArgoNeuT::postOpenFile);
    reg.sPreProcessEvent.watch (this, &DetectorPropertiesServiceArgoNeuT::preProcessEvent);
  }

  //------------------------------------------------------------
  double DetectorPropertiesServiceArgoNeuT::ConvertTDCToTicks(double tdc) const
  {
    return lar::providerFrom<detinfo::DetectorClocksService>()->TPCTDC2Tick(tdc);
  }

  //--------------------------------------------------------------
  double DetectorPropertiesServiceArgoNeuT::ConvertTicksToTDC(double ticks) const
  {
    return lar::providerFrom<detinfo::DetectorClocksService>()->TPCTick2TDC(ticks);
  }

  //--------------------------------------------------------------------
  void DetectorPropertiesServiceArgoNeuT::reconfigure(fhicl::ParameterSet const& p)
  {
    //fSamplingRate             = p.get< double        >("SamplingRate"     );
    double d;
    int    i;
    bool   b;
    if(p.get_if_present<double>("SamplingRate",d))
      throw cet::exception(__FUNCTION__) << "SamplingRate is a deprecated fcl parameter for DetectorPropertiesServiceArgoNeuT!";
    if(p.get_if_present<int>("TriggerOffset",i))
      throw cet::exception(__FUNCTION__) << "TriggerOffset is a deprecated fcl parameter for DetectorPropertiesServiceArgoNeuT!";
    if(p.get_if_present<bool>("InheritTriggerOffset",b))
      throw cet::exception(__FUNCTION__) << "InheritTriggerOffset is a deprecated fcl parameter for DetectorPropertiesServiceArgoNeuT!";

    fElectronsToADC    	      = p.get< double 	     >("ElectronsToADC"   );
    fNumberTimeSamples 	      = p.get< unsigned int >("NumberTimeSamples");
    fReadOutWindowSize 	      = p.get< unsigned int >("ReadOutWindowSize");
    fTimeOffsetU       	      = p.get< double 	     >("TimeOffsetU"      );
    fTimeOffsetV       	      = p.get< double 	     >("TimeOffsetV"      );
    fTimeOffsetZ       	      = p.get< double 	     >("TimeOffsetZ"      );
    fInheritNumberTimeSamples = p.get<bool           >("InheritNumberTimeSamples", false);

    fSimpleBoundary           = p.get<bool           >("SimpleBoundaryProcess", true);

    fXTicksParamsLoaded = false;

    fTPCClock = lar::providerFrom<detinfo::DetectorClocksService>()->TPCClock();

    // Save the parameter set.
    fPS = p;

    return;
  }

  //-------------------------------------------------------------
  void DetectorPropertiesServiceArgoNeuT::preProcessEvent(const art::Event& evt, art::ScheduleContext)
  {
    // Make sure TPC Clock is updated with DetectorClocksService (though in principle it shouldn't change)
    fTPCClock = lar::providerFrom<detinfo::DetectorClocksService>()->TPCClock();
  }

//---------------------------------------------------------------------------------
void DetectorPropertiesServiceArgoNeuT::checkDBstatus() const
{
  bool fToughErrorTreatment= art::ServiceHandle<util::DatabaseUtil const>()->ToughErrorTreatment();
  bool fShouldConnect =  art::ServiceHandle<util::DatabaseUtil const>()->ShouldConnect();
  //Have not read from DB, should read and requested tough treatment
    if(!fAlreadyReadFromDB && fToughErrorTreatment && fShouldConnect )
      throw cet::exception("DetectorPropertiesServiceArgoNeuT") << " Extracting values from DetectorPropertiesServiceArgoNeuT before they "
              << " have been read in from database. \n "
              << "Set ToughErrorTreatment or ShouldConnect "
              << " to false in databaseutil.fcl if you want "
              << " to avoid this. \n";
   //Have not read from DB, should read and requested soft treatment
    else if(!fAlreadyReadFromDB && !fToughErrorTreatment && fShouldConnect )
      mf::LogWarning("DetectorPropertiesServiceArgoNeuT") <<  "!!! Extracting values from DetectorPropertiesServiceArgoNeuT before they "
              << " have been read in from the database. \n "
              << " You may not be using the correct values of "
              << " T0!"
              << " You should not be initializing"
              << " Database originating values in BeginJob()s or constructors."
              << " You have been warned !!! \n ";

    //In other cases, either already read from DB, or should not connect so it doesn't matter
}

//------------------------------------------------------------------------------------//
int  DetectorPropertiesServiceArgoNeuT::TriggerOffset()     const
{
  return fTPCClock.Ticks(lar::providerFrom<detinfo::DetectorClocksService>()->TriggerOffsetTPC() * -1.);
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

  double DetectorPropertiesServiceArgoNeuT::ConvertXToTicks(double X, int p, int t, int c) const
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return (X / (fXTicksCoefficient * fDriftDirection.at(c).at(t)) +  fXTicksOffsets.at(c).at(t).at(p) );
  }



  //-------------------------------------------------------------------
  // Take a cooridnate in ticks, and convert to an x position
  // assuming event deposit occured at t=0

  double  DetectorPropertiesServiceArgoNeuT::ConvertTicksToX(double ticks, int p, int t, int c) const
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return (ticks - fXTicksOffsets.at(c).at(t).at(p)) * fXTicksCoefficient * fDriftDirection.at(c).at(t);
  }


  //--------------------------------------------------------------------
  // Recalculte x<-->ticks conversion parameters from detector constants

  void DetectorPropertiesServiceArgoNeuT::CalculateXTicksParams() const
  {
    art::ServiceHandle<geo::Geometry const>            geo;

    double samplingRate   = SamplingRate();
    double efield         = Efield();
    double temperature    = fLP->Temperature();
    double driftVelocity  = DriftVelocity(efield, temperature);

    fXTicksCoefficient    = 0.001 * driftVelocity * samplingRate;

    double triggerOffset  = TriggerOffset();

    fXTicksOffsets.clear();
    fXTicksOffsets.resize(geo->Ncryostats());

    fDriftDirection.clear();
    fDriftDirection.resize(geo->Ncryostats());

    for(size_t cstat = 0; cstat < geo->Ncryostats(); ++cstat){
      fXTicksOffsets[cstat].resize(geo->Cryostat(cstat).NTPC());
      fDriftDirection[cstat].resize(geo->Cryostat(cstat).NTPC());

      for(size_t tpc = 0; tpc < geo->Cryostat(cstat).NTPC(); ++tpc) {
	const geo::TPCGeo& tpcgeom = geo->Cryostat(cstat).TPC(tpc);

        const double dir((tpcgeom.DriftDirection() == geo::kNegX) ? +1.0 :-1.0);
        fDriftDirection[cstat][tpc] = dir;

	int nplane = tpcgeom.Nplanes();
	fXTicksOffsets[cstat][tpc].resize(nplane, 0.);
	for(int plane = 0; plane < nplane; ++plane) {
	  const geo::PlaneGeo& pgeom = tpcgeom.Plane(plane);


	  // Get field in gap between planes
	  double efieldgap[3];
	  double driftVelocitygap[3];
	  double fXTicksCoefficientgap[3];
	  for (int igap = 0; igap<3; ++igap){
	    efieldgap[igap] = Efield(igap);
	    driftVelocitygap[igap] = DriftVelocity(efieldgap[igap], temperature);
	    fXTicksCoefficientgap[igap] = 0.001 * driftVelocitygap[igap] * samplingRate;
	  }

	  // Calculate geometric time offset.
	  // only works if xyz[0]<=0
	  const double* xyz = tpcgeom.PlaneLocation(0);

	  fXTicksOffsets[cstat][tpc][plane] = -xyz[0]/(dir * fXTicksCoefficient) + triggerOffset;

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
	    throw cet::exception("DetectorPropertiesServiceArgoNeuT") << "Bad view = "
						       << view << "\n" ;
	}
      }
    }

    fXTicksParamsLoaded=true;
  }

  //--------------------------------------------------------------------
  // Get scale factor for x<-->ticks

  double DetectorPropertiesServiceArgoNeuT::GetXTicksCoefficient(int t, int c) const
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return fXTicksCoefficient * fDriftDirection.at(c).at(t);
  }

  //--------------------------------------------------------------------
  // Get scale factor for x<-->ticks

  double DetectorPropertiesServiceArgoNeuT::GetXTicksCoefficient() const
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return fXTicksCoefficient;
  }

  //--------------------------------------------------------------------
  //  Get offset for x<-->ticks

  double DetectorPropertiesServiceArgoNeuT::GetXTicksOffset(int p, int t, int c) const
  {
    if(!fXTicksParamsLoaded) CalculateXTicksParams();
    return fXTicksOffsets.at(c).at(t).at(p);
  }

  //--------------------------------------------------------------------
  //  Callback called after input file is opened.

  void DetectorPropertiesServiceArgoNeuT::postOpenFile(const std::string& filename)
  {
    // Use this method to figure out whether to inherit configuration
    // parameters from previous jobs.
    //
    // There is no way currently to correlate parameter sets saved in
    // sqlite RootFileDB with process history (from MetaData tree).
    // Therefore, we use the approach of scanning every historical
    // parameter set in RootFileDB, and finding all parameter sets
    // that appear to be DetectorPropertiesServiceArgoNeuT configurations.  If all
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

    if(!fInheritNumberTimeSamples)
      return;

    // The only way to access art service metadata from the input file
    // is to open it as a separate TFile object.  Do that now.

    if(filename.size() != 0) {

      TFile* file = TFile::Open(filename.c_str(), "READ");
      if(file != 0 && !file->IsZombie() && file->IsOpen()) {

	// Open the sqlite datatabase.

	art::SQLite3Wrapper sqliteDB(file, "RootFileDB");

	// Loop over all stored ParameterSets.

	unsigned int iNumberTimeSamples = 0;  // Combined value of NumberTimeSamples.
	unsigned int nNumberTimeSamples = 0;  // Number of NumberTimeSamples parameters seen.

	sqlite3_stmt * stmt = 0;
	sqlite3_prepare_v2(sqliteDB, "SELECT PSetBlob from ParameterSets;", -1, &stmt, NULL);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
	  fhicl::ParameterSet ps;
	  fhicl::make_ParameterSet(reinterpret_cast<char const *>(sqlite3_column_text(stmt, 0)), ps);
	  // Is this a DetectorPropertiesServiceArgoNeuT parameter set?

	  bool psok = isDetectorPropertiesServiceArgoNeuT(ps);
	  if(psok) {

	    // Check NumberTimeSamples

	    if(fInheritNumberTimeSamples) {
	      unsigned int newNumberTimeSamples = ps.get<unsigned int>("NumberTimeSamples");

	      // Ignore parameter values that match the current configuration.

	      if(newNumberTimeSamples != fPS.get<unsigned int>("NumberTimeSamples")) {
		if(nNumberTimeSamples == 0)
		  iNumberTimeSamples = newNumberTimeSamples;
		else if(newNumberTimeSamples != iNumberTimeSamples) {
		  throw cet::exception("DetectorPropertiesServiceArgoNeuT")
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

	if(fInheritNumberTimeSamples &&
	   nNumberTimeSamples != 0 &&
	   iNumberTimeSamples != fNumberTimeSamples) {
	  mf::LogInfo("DetectorPropertiesServiceArgoNeuT")
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
  //  Determine whether a parameter set is a DetectorPropertiesServiceArgoNeuT configuration.

  bool DetectorPropertiesServiceArgoNeuT::isDetectorPropertiesServiceArgoNeuT(const fhicl::ParameterSet& ps)
  {
    // This method uses heuristics to determine whether the parameter
    // set passed as argument is a DetectorPropertiesServiceArgoNeuT configuration
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

  DEFINE_ART_SERVICE_INTERFACE_IMPL(DetectorPropertiesServiceArgoNeuT, detinfo::DetectorPropertiesService)

} // namespace util
