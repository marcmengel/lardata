////////////////////////////////////////////////////////////////////////
//
//  \file DetectorProperties.cxx
//
// Separation of service from Detector info class:
// jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
// Framework includes

#include <cassert>

// LArSoft includes
#include "DetectorInfo/DetectorPropertiesStandard.h"
#include "Geometry/Geometry.h"
#include "Geometry/CryostatGeo.h"
#include "Geometry/TPCGeo.h"
#include "Geometry/PlaneGeo.h"
#include "SimpleTypesAndConstants/PhysicalConstants.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

// Art includes
#include "art/Persistency/RootDB/SQLite3Wrapper.h"
#include "fhiclcpp/make_ParameterSet.h"

namespace detinfo{

  //--------------------------------------------------------------------
  DetectorPropertiesStandard::DetectorPropertiesStandard() :
    fLP(0), fClocks(0), fGeo(0)
  {

  }
    
  //--------------------------------------------------------------------
  DetectorPropertiesStandard::DetectorPropertiesStandard(fhicl::ParameterSet const& pset,
					 const geo::GeometryCore* geo,
					 const detinfo::LArProperties* lp,
					 const detinfo::DetectorClocks* c):
    fLP(lp), fClocks(c), fGeo(geo)
  {
    Configure(pset);
    
    fTPCClock = fClocks->TPCClock();
    
  }
  
  //--------------------------------------------------------------------
  DetectorPropertiesStandard::~DetectorPropertiesStandard() 
  {
    
  }

  //--------------------------------------------------------------------
  bool DetectorPropertiesStandard::Update(uint64_t t) 
  {

    CalculateXTicksParams();
    return true;
  }

  //--------------------------------------------------------------------
  bool DetectorPropertiesStandard::UpdateClocks(const detinfo::DetectorClocks* clks) 
  {
    fClocks = clks;
    
    fTPCClock = fClocks->TPCClock();
    CalculateXTicksParams();
    return true;
  }
  
  //------------------------------------------------------------
  double DetectorPropertiesStandard::ConvertTDCToTicks(double tdc) const
  {
    if (fClocks!=0) throw cet::exception(__FUNCTION__) << "DetectorClocks is uninitialized!";

    return fClocks->TPCTDC2Tick(tdc);
  }
  
  //--------------------------------------------------------------
  double DetectorPropertiesStandard::ConvertTicksToTDC(double ticks) const
  {
    if (fClocks!=0) throw cet::exception(__FUNCTION__) << "DetectorClocks is uninitialized!";
    return fClocks->TPCTick2TDC(ticks);
  }
  
  //--------------------------------------------------------------------
  void DetectorPropertiesStandard::Configure(fhicl::ParameterSet const& p)
  {
    //fSamplingRate             = p.get< double        >("SamplingRate"     );
    double d;
    int    i;
    bool   b;
    if(p.get_if_present<double>("SamplingRate",d))
      throw cet::exception(__FUNCTION__) << "SamplingRate is a deprecated fcl parameter for DetectorPropertiesStandard!";
    if(p.get_if_present<int>("TriggerOffset",i))
      throw cet::exception(__FUNCTION__) << "TriggerOffset is a deprecated fcl parameter for DetectorPropertiesStandard!";
    if(p.get_if_present<bool>("InheritTriggerOffset",b))
      throw cet::exception(__FUNCTION__) << "InheritTriggerOffset is a deprecated fcl parameter for DetectorPropertiesStandard!";
    
    SetEfield(p.get< std::vector<double> >("Efield"));
    fElectronlifetime         = p.get< double       >("Electronlifetime");
    fNumberTimeSamples        = p.get< unsigned int >("NumberTimeSamples");
    fElectronsToADC    	      = p.get< double 	    >("ElectronsToADC"   );
    fNumberTimeSamples 	      = p.get< unsigned int >("NumberTimeSamples");
    fReadOutWindowSize 	      = p.get< unsigned int >("ReadOutWindowSize");
    fTimeOffsetU       	      = p.get< double 	    >("TimeOffsetU"      );
    fTimeOffsetV       	      = p.get< double 	    >("TimeOffsetV"      );
    fTimeOffsetZ       	      = p.get< double 	    >("TimeOffsetZ"      );
    fInheritNumberTimeSamples = p.get<bool          >("InheritNumberTimeSamples", false);

    CalculateXTicksParams();
    
    return;
  }
  
  
//------------------------------------------------------------------------------------//
  double DetectorPropertiesStandard::Efield(unsigned int planegap) const
{
  if(planegap >= fEfield.size())
    throw cet::exception("LArPropertiesStandard") << "requesting Electric field in a plane gap that is not defined\n";
  
  return fEfield[planegap];
}

//------------------------------------------------------------------------------------//
  double DetectorPropertiesStandard::DriftVelocity(double efield, double temperature) const {

  // Drift Velocity as a function of Electric Field and LAr Temperature
  // from : W. Walkowiak, NIM A 449 (2000) 288-294
  //
  // Efield should have units of kV/cm
  // Temperature should have units of Kelvin

  // Default Efield, use internal value.
  if(efield == 0.)
    efield = Efield();
  //
  if(efield > 4.0)
    mf::LogWarning("LArPropertiesStandard") << "DriftVelocity Warning! : E-field value of "
				    << efield
				    << " kV/cm is outside of range covered by drift"
				    << " velocity parameterization. Returned value"
				    << " may not be correct";


  // Default temperature use internal value.
  if(temperature == 0.)
    temperature = fLP->Temperature();

  if(temperature < 87.0 || temperature > 94.0)
    mf::LogWarning("LArPropertiesStandard") << "DriftVelocity Warning! : Temperature value of "
				    << temperature
				    << " K is outside of range covered by drift velocity"
				    << " parameterization. Returned value may not be"
				    << " correct";




  double tshift = -87.203+temperature;
  double xFit = 0.0938163-0.0052563*tshift-0.0001470*tshift*tshift;
  double uFit = 5.18406+0.01448*tshift-0.003497*tshift*tshift-0.000516*tshift*tshift*tshift;
  double vd;


// Icarus Parameter Set, use as default
  double  P1 = -0.04640; // K^-1
  double  P2 = 0.01712;  // K^-1
  double  P3 = 1.88125;   // (kV/cm)^-1
  double  P4 =  0.99408;    // kV/cm
  double  P5 =  0.01172;   // (kV/cm)^-P6
  double  P6 =  4.20214;
  double  T0 =  105.749;  // K
      // Walkowiak Parameter Set
  double    P1W = -0.01481; // K^-1
  double  P2W = -0.0075;  // K^-1
  double   P3W =  0.141;   // (kV/cm)^-1
  double   P4W =  12.4;    // kV/cm
  double   P5W =  1.627;   // (kV/cm)^-P6
  double   P6W =  0.317;
  double   T0W =  90.371;  // K

// From Craig Thorne . . . currently not documented
// smooth transition from linear at small fields to 
//     icarus fit at most fields to Walkowiak at very high fields
   if (efield < xFit) vd=efield*uFit;
   else if (efield<0.619) { 
     vd = ((P1*(temperature-T0)+1)
	       *(P3*efield*std::log(1+P4/efield) + P5*std::pow(efield,P6))
	       +P2*(temperature-T0));
   }
   else if (efield<0.699) {
     vd = 12.5*(efield-0.619)*((P1W*(temperature-T0W)+1)
	       *(P3W*efield*std::log(1+P4W/efield) + P5W*std::pow(efield,P6W))
	       +P2W*(temperature-T0W))+
       12.5*(0.699-efield)*((P1*(temperature-T0)+1)
	       *(P3*efield*std::log(1+P4/efield) + P5*std::pow(efield,P6))
	       +P2*(temperature-T0));
   }
   else {
     vd = ((P1W*(temperature-T0W)+1)
	       *(P3W*efield*std::log(1+P4W/efield) + P5W*std::pow(efield,P6W))
	       +P2W*(temperature-T0W));     
   }

  vd /= 10.;

  return vd; // in cm/us
}

  //----------------------------------------------------------------------------------
  // The below function assumes that the user has applied the lifetime correction and
  // effective pitch between the wires (usually after 3D reconstruction). Using with
  // mean wire pitch will not give correct results.
  // parameters:
  //  dQdX in electrons/cm, charge (amplitude or integral obtained) divided by
  //         effective pitch for a given 3D track.
  // returns dEdX in MeV/cm
  double DetectorPropertiesStandard::BirksCorrection(double dQdx) const
  {
    // Correction for charge quenching using parameterization from
    // S.Amoruso et al., NIM A 523 (2004) 275
    
    double  A3t    = util::kRecombA;
    double  K3t    = util::kRecombk;                     // in KV/cm*(g/cm^2)/MeV
    double  rho    = fLP->Density();                    // LAr density in g/cm^3
    double Wion    = 1000./util::kGeVToElectrons;        // 23.6 eV = 1e, Wion in MeV/e
    double Efield  = this->Efield();                     // Electric Field in the drift region in KV/cm
    K3t           /= rho;                                // KV/MeV
    double dEdx    = dQdx/(A3t/Wion-K3t/Efield*dQdx);    //MeV/cm
    
    return dEdx;
  }  
  
  //----------------------------------------------------------------------------------
  // Modified Box model correction 
  double DetectorPropertiesStandard::ModBoxCorrection(double dQdx) const
  {
    // Modified Box model correction has better behavior than the Birks
    // correction at high values of dQ/dx.
    double  rho    = fLP->Density();                    // LAr density in g/cm^3
    double Wion    = 1000./util::kGeVToElectrons;        // 23.6 eV = 1e, Wion in MeV/e
    double Efield  = this->Efield();                     // Electric Field in the drift region in KV/cm
    double Beta    = util::kModBoxB / (rho * Efield);
    double Alpha   = util::kModBoxA;
    double dEdx = (exp(Beta * Wion * dQdx ) - Alpha) / Beta;
    
    return dEdx;
    
  }
  
  //------------------------------------------------------------------------------------//
  int  DetectorPropertiesStandard::TriggerOffset()     const 
  {
    if (fClocks!=0) throw cet::exception(__FUNCTION__) << "DetectorClocks is uninitialized!";
    return fTPCClock.Ticks(fClocks->TriggerOffsetTPC() * -1.);
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
 
  double DetectorPropertiesStandard::ConvertXToTicks(double X, int p, int t, int c) const
  {
    return (X / (fXTicksCoefficient * fDriftDirection.at(c).at(t)) +  fXTicksOffsets.at(c).at(t).at(p) );
  }
  


  //-------------------------------------------------------------------
  // Take a cooridnate in ticks, and convert to an x position
  // assuming event deposit occured at t=0
 
  double  DetectorPropertiesStandard::ConvertTicksToX(double ticks, int p, int t, int c) const
  {
    return (ticks - fXTicksOffsets.at(c).at(t).at(p)) * fXTicksCoefficient * fDriftDirection.at(c).at(t);  
  }
  

  //--------------------------------------------------------------------
  void DetectorPropertiesStandard::CheckIfConfigured()
  {
    if (fGeo!=0) throw cet::exception(__FUNCTION__) << "Geometry is uninitialized!";
    if (fLP!=0) throw cet::exception(__FUNCTION__) << "LArPropertiesStandard is uninitialized!";
    if (fClocks!=0) throw cet::exception(__FUNCTION__) << "DetectorClocks is uninitialized!";
  }
  
  
  //--------------------------------------------------------------------
  // Recalculte x<-->ticks conversion parameters from detector constants
  
  void DetectorPropertiesStandard::CalculateXTicksParams()
  {
    CheckIfConfigured();
    
    double samplingRate   = SamplingRate();
    double efield         = Efield();
    double temperature    = fLP->Temperature();
    double driftVelocity  = DriftVelocity(efield, temperature);
    
    fXTicksCoefficient    = 0.001 * driftVelocity * samplingRate;

    double triggerOffset  = TriggerOffset();

    fXTicksOffsets.clear();
    fXTicksOffsets.resize(fGeo->Ncryostats());

    fDriftDirection.clear();
    fDriftDirection.resize(fGeo->Ncryostats());

    for(size_t cstat = 0; cstat < fGeo->Ncryostats(); ++cstat){
      fXTicksOffsets[cstat].resize(fGeo->Cryostat(cstat).NTPC());
      fDriftDirection[cstat].resize(fGeo->Cryostat(cstat).NTPC());

      for(size_t tpc = 0; tpc < fGeo->Cryostat(cstat).NTPC(); ++tpc) {
	const geo::TPCGeo& tpcgeom = fGeo->Cryostat(cstat).TPC(tpc);

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
	    throw cet::exception(__FUNCTION__) << "Bad view = "
						       << view << "\n" ;
	}	
      }
    }

  }

  //--------------------------------------------------------------------
  // Get scale factor for x<-->ticks

  double DetectorPropertiesStandard::GetXTicksCoefficient(int t, int c) const
  {
    return fXTicksCoefficient * fDriftDirection.at(c).at(t);
  }

  //--------------------------------------------------------------------
  // Get scale factor for x<-->ticks

  double DetectorPropertiesStandard::GetXTicksCoefficient() const
  {
    return fXTicksCoefficient;
  }

  //--------------------------------------------------------------------
  //  Get offset for x<-->ticks

  double DetectorPropertiesStandard::GetXTicksOffset(int p, int t, int c) const
  {
    return fXTicksOffsets.at(c).at(t).at(p);	
  }

} // namespace
