////////////////////////////////////////////////////////////////////////
//
//  LArProperties
//
////////////////////////////////////////////////////////////////////////
// Framework includes

// C++ language includes
#include <cmath>
#include <iostream>

// LArSoft includes
#include "DetectorInfo/LArPropertiesStandard.h"
#include "SimpleTypesAndConstants/PhysicalConstants.h"

// ROOT includes
#include "TMath.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/exception.h"

//-----------------------------------------------
detinfo::LArPropertiesStandard::LArPropertiesStandard()
{
  fIsConfigured = false;
}

//-----------------------------------------------
detinfo::LArPropertiesStandard::LArPropertiesStandard(fhicl::ParameterSet const& pset)
{
  fIsConfigured = this->Configure(pset);
}

//------------------------------------------------
detinfo::LArPropertiesStandard::~LArPropertiesStandard()
{
}

//------------------------------------------------
bool detinfo::LArPropertiesStandard::Configure(fhicl::ParameterSet const& pset)
{  
  this->SetTemperature      (pset.get< double >("Temperature"));
  this->SetElectronlifetime (pset.get< double >("Electronlifetime"));
  this->SetRadiationLength  (pset.get< double >("RadiationLength" ));
  this->SetAtomicNumber     (pset.get< double >("AtomicNumber"));
  this->SetAtomicMass       (pset.get< double >("AtomicMass"));
  this->SetMeanExcitationEnergy (pset.get< double >("ExcitationEnergy"));
  this->SetSa               (pset.get< double >("SternheimerA"));
  this->SetSk               (pset.get< double >("SternheimerK"));
  this->SetSx0              (pset.get< double >("SternheimerX0"));
  this->SetSx1              (pset.get< double >("SternheimerX1"));
  this->SetScbar            (pset.get< double >("SternheimerCbar"));

  this->SetArgon39DecayRate  (pset.get< double >("Argon39DecayRate"));

  this->SetFastScintEnergies (pset.get< std::vector<double> >("FastScintEnergies"));
  this->SetFastScintSpectrum (pset.get< std::vector<double> >("FastScintSpectrum"));
  this->SetSlowScintEnergies (pset.get< std::vector<double> >("SlowScintEnergies"));
  this->SetSlowScintSpectrum (pset.get< std::vector<double> >("SlowScintSpectrum"));
  this->SetAbsLengthEnergies (pset.get< std::vector<double> >("AbsLengthEnergies"));
  this->SetAbsLengthSpectrum (pset.get< std::vector<double> >("AbsLengthSpectrum"));
  this->SetRIndexEnergies    (pset.get< std::vector<double> >("RIndexEnergies"   ));
  this->SetRIndexSpectrum    (pset.get< std::vector<double> >("RIndexSpectrum"   ));
  this->SetRayleighEnergies  (pset.get< std::vector<double> >("RayleighEnergies" ));
  this->SetRayleighSpectrum  (pset.get< std::vector<double> >("RayleighSpectrum" ));

  this->SetScintResolutionScale (pset.get<double>("ScintResolutionScale"));
  this->SetScintFastTimeConst   (pset.get<double>("ScintFastTimeConst"));
  this->SetScintSlowTimeConst   (pset.get<double>("ScintSlowTimeConst"));
  this->SetScintBirksConstant   (pset.get<double>("ScintBirksConstant"));
  this->SetScintByParticleType  (pset.get<bool>("ScintByParticleType"));
  this->SetScintYield   (pset.get<double>("ScintYield"));
  this->SetScintPreScale(pset.get<double>("ScintPreScale"));
  this->SetScintYieldRatio      (pset.get<double>("ScintYieldRatio"));

  if(ScintByParticleType()){
    this->SetProtonScintYield(pset.get<double>("ProtonScintYield"));
    this->SetProtonScintYieldRatio   (pset.get<double>("ProtonScintYieldRatio"));
    this->SetMuonScintYield  (pset.get<double>("MuonScintYield"));
    this->SetMuonScintYieldRatio     (pset.get<double>("MuonScintYieldRatio"));
    this->SetPionScintYield  (pset.get<double>("PionScintYield"));
    this->SetPionScintYieldRatio     (pset.get<double>("PionScintYieldRatio"));
    this->SetKaonScintYield  (pset.get<double>("KaonScintYield"));
    this->SetKaonScintYieldRatio     (pset.get<double>("KaonScintYieldRatio"));
    this->SetElectronScintYield      (pset.get<double>("ElectronScintYield"));
    this->SetElectronScintYieldRatio (pset.get<double>("ElectronScintYieldRatio"));
    this->SetAlphaScintYield (pset.get<double>("AlphaScintYield"));
    this->SetAlphaScintYieldRatio    (pset.get<double>("AlphaScintYieldRatio"));
  }
  
  this->SetEnableCerenkovLight  (pset.get<bool>("EnableCerenkovLight"));
  
  this->SetReflectiveSurfaceNames(pset.get<std::vector<std::string> >("ReflectiveSurfaceNames"));
  this->SetReflectiveSurfaceEnergies(pset.get<std::vector<double> >("ReflectiveSurfaceEnergies"));
  this->SetReflectiveSurfaceReflectances(pset.get<std::vector<std::vector<double> > >("ReflectiveSurfaceReflectances"));
  this->SetReflectiveSurfaceDiffuseFractions(pset.get<std::vector<std::vector<double> > >("ReflectiveSurfaceDiffuseFractions"));

  fIsConfigured = true;


  return true;
}

//------------------------------------------------
bool detinfo::LArPropertiesStandard::Update(uint64_t ts) 
{
  if (ts == 0) return false;

  return true;
}

//------------------------------------------------
// temperature is assumed to be in degrees Kelvin
// density is nearly a linear function of temperature.  
// See the NIST tables for details
// slope is between -6.2 and -6.1, intercept is 1928 kg/m^3
// this parameterization will be good to better than 0.5%.
// density is returned in g/cm^3
double detinfo::LArPropertiesStandard::Density(double temperature) const
{
  // Default temperature use internal value.
  if(temperature == 0.)
    temperature = Temperature();

  double density = -0.00615*temperature + 1.928;

  return density;
}


//------------------------------------------------------------------------------------//
double detinfo::LArPropertiesStandard::Temperature() const
{
  return fTemperature;
}

//----------------------------------------------------------------------------------
// Restricted mean energy loss (dE/dx) in units of MeV/cm.
//
// For unrestricted mean energy loss, set tcut = 0, or tcut large.
//
// Arguments:
//
// mom  - Momentum of incident particle in GeV/c.
// mass - Mass of incident particle in GeV/c^2.
// tcut - Maximum kinetic energy of delta rays (MeV).
//
// Returned value is positive.
//
// Based on Bethe-Bloch formula as contained in particle data book.
// Material parameters (stored in larproperties.fcl) are taken from
// pdg web site http://pdg.lbl.gov/AtomicNuclearProperties/
//
double detinfo::LArPropertiesStandard::Eloss(double mom, double mass, double tcut) const
{
  // Some constants.

  double K = 0.307075;     // 4 pi N_A r_e^2 m_e c^2 (MeV cm^2/mol).
  double me = 0.510998918; // Electron mass (MeV/c^2).

  // Calculate kinematic quantities.

  double bg = mom / mass;           // beta*gamma.
  double gamma = sqrt(1. + bg*bg);  // gamma.
  double beta = bg / gamma;         // beta (velocity).
  double mer = 0.001 * me / mass;   // electron mass / mass of incident particle.
  double tmax = 2.*me* bg*bg / (1. + 2.*gamma*mer + mer*mer);  // Maximum delta ray energy (MeV).

  // Make sure tcut does not exceed tmax.

  if(tcut == 0. || tcut > tmax)
    tcut = tmax;

  // Calculate density effect correction (delta).

  double x = std::log10(bg);
  double delta = 0.;
  if(x >= fSx0) {
    delta = 2. * std::log(10.) * x - fScbar;
    if(x < fSx1)
      delta += fSa * std::pow(fSx1 - x, fSk);
  }

  // Calculate stopping number.

  double B = 0.5 * std::log(2.*me*bg*bg*tcut / (1.e-12 * fI*fI))
    - 0.5 * beta*beta * (1. + tcut / tmax) - 0.5 * delta;

  // Don't let the stopping number become negative.

  if(B < 1.)
    B = 1.;

  // Calculate dE/dx.

  double dedx = Density() * K*fZ*B / (fA * beta*beta);

  // Done.

  return dedx;
}

//----------------------------------------------------------------------------------
// Energy loss fluctuation (sigma_E^2 / length in MeV^2/cm).
//
// Arguments:
//
// mom  - Momentum of incident particle in GeV/c.
//
// Based on Bichsel formula referred to but not given in pdg.
//
double detinfo::LArPropertiesStandard::ElossVar(double mom, double mass) const
{
  // Some constants.

  double K = 0.307075;     // 4 pi N_A r_e^2 m_e c^2 (MeV cm^2/mol).
  double me = 0.510998918; // Electron mass (MeV/c^2).

  // Calculate kinematic quantities.

  double bg = mom / mass;          // beta*gamma.
  double gamma2 = 1. + bg*bg;      // gamma^2.
  double beta2 = bg*bg / gamma2;   // beta^2.

  // Calculate final result.

  double result = gamma2 * (1. - 0.5 * beta2) * me * (fZ / fA) * K * Density();
  return result;
}

//---------------------------------------------------------------------------------
std::map<double,double> detinfo::LArPropertiesStandard::FastScintSpectrum() const
{
  if(fFastScintSpectrum.size()!=fFastScintEnergies.size()){
    throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
      << "The vectors specifying the fast scintillation spectrum are "
      << " different sizes - " << fFastScintSpectrum.size()
      << " " << fFastScintEnergies.size();
  }

  std::map<double, double> ToReturn;
  for(size_t i=0; i!=fFastScintSpectrum.size(); ++i)
    ToReturn[fFastScintEnergies.at(i)]=fFastScintSpectrum.at(i);

  return ToReturn;
}

//---------------------------------------------------------------------------------
std::map<double, double> detinfo::LArPropertiesStandard::SlowScintSpectrum() const
{
  if(fSlowScintSpectrum.size()!=fSlowScintEnergies.size()){
      throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
  << "The vectors specifying the slow scintillation spectrum are "
  << " different sizes - " << fFastScintSpectrum.size()
  << " " << fFastScintEnergies.size();
    }

  std::map<double, double> ToReturn;
  for(size_t i=0; i!=fSlowScintSpectrum.size(); ++i)
    ToReturn[fSlowScintEnergies.at(i)]=fSlowScintSpectrum.at(i);

  return ToReturn;
}

//---------------------------------------------------------------------------------
std::map<double, double> detinfo::LArPropertiesStandard::RIndexSpectrum() const
{
  if(fRIndexSpectrum.size()!=fRIndexEnergies.size()){
      throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
  << "The vectors specifying the RIndex spectrum are "
  << " different sizes - " << fRIndexSpectrum.size()
  << " " << fRIndexEnergies.size();
  }

  std::map<double, double> ToReturn;
  for(size_t i=0; i!=fRIndexSpectrum.size(); ++i)
    ToReturn[fRIndexEnergies.at(i)]=fRIndexSpectrum.at(i);

  return ToReturn;
}


//---------------------------------------------------------------------------------
std::map<double, double> detinfo::LArPropertiesStandard::AbsLengthSpectrum() const
{
  if(fAbsLengthSpectrum.size()!=fAbsLengthEnergies.size()){
    throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
      << "The vectors specifying the Abs Length spectrum are "
      << " different sizes - " << fAbsLengthSpectrum.size()
      << " " << fAbsLengthEnergies.size();
  }

  std::map<double, double> ToReturn;
  for(size_t i=0; i!=fAbsLengthSpectrum.size(); ++i)
    ToReturn[fAbsLengthEnergies.at(i)]=fAbsLengthSpectrum.at(i);

  return ToReturn;
}

//---------------------------------------------------------------------------------
std::map<double, double> detinfo::LArPropertiesStandard::RayleighSpectrum() const
{
  if(fRayleighSpectrum.size()!=fRayleighEnergies.size()){
    throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
      << "The vectors specifying the rayleigh spectrum are "
      << " different sizes - " << fRayleighSpectrum.size()
      << " " << fRayleighEnergies.size();
  }

  std::map<double, double> ToReturn;
  for(size_t i=0; i!=fRayleighSpectrum.size(); ++i)
    ToReturn[fRayleighEnergies.at(i)]=fRayleighSpectrum.at(i);

  return ToReturn;
}

//---------------------------------------------------------------------------------
std::map<std::string, std::map<double,double> > detinfo::LArPropertiesStandard::SurfaceReflectances() const
{
  std::map<std::string, std::map<double, double> > ToReturn;

  if(fReflectiveSurfaceNames.size()!=fReflectiveSurfaceReflectances.size()){
    throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
      << "The vectors specifying the surface reflectivities "
      << "do not have consistent sizes";
  }
  for(size_t i=0; i!=fReflectiveSurfaceNames.size(); ++i){
    if(fReflectiveSurfaceEnergies.size()!=fReflectiveSurfaceReflectances.at(i).size()){
      throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
  << "The vectors specifying the surface reflectivities do not have consistent sizes";
    }
  }
  for(size_t iName=0; iName!=fReflectiveSurfaceNames.size(); ++iName)
    for(size_t iEnergy=0; iEnergy!=fReflectiveSurfaceEnergies.size(); ++iEnergy)
      ToReturn[fReflectiveSurfaceNames.at(iName)][fReflectiveSurfaceEnergies.at(iEnergy)]=fReflectiveSurfaceReflectances[iName][iEnergy];

  return ToReturn;

}

//---------------------------------------------------------------------------------
std::map<std::string, std::map<double,double> > detinfo::LArPropertiesStandard::SurfaceReflectanceDiffuseFractions() const
{
  std::map<std::string, std::map<double, double> > ToReturn;

  if(fReflectiveSurfaceNames.size()!=fReflectiveSurfaceDiffuseFractions.size()){
    throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
      << "The vectors specifying the surface reflectivities do not have consistent sizes";
  }
  for(size_t i=0; i!=fReflectiveSurfaceNames.size(); ++i){
    if(fReflectiveSurfaceEnergies.size()!=fReflectiveSurfaceDiffuseFractions.at(i).size()){
      throw cet::exception("Incorrect vector sizes in LArPropertiesStandard")
  << "The vectors specifying the surface reflectivities do not have consistent sizes";

    }
  }
  for(size_t iName=0; iName!=fReflectiveSurfaceNames.size(); ++iName)
    for(size_t iEnergy=0; iEnergy!=fReflectiveSurfaceEnergies.size(); ++iEnergy)
      ToReturn[fReflectiveSurfaceNames.at(iName)][fReflectiveSurfaceEnergies.at(iEnergy)]=fReflectiveSurfaceDiffuseFractions[iName][iEnergy];

  return ToReturn;
}
