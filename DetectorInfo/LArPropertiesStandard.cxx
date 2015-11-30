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
bool detinfo::LArPropertiesStandard::Configure(fhicl::ParameterSet const& pset)
{  
  this->SetRadiationLength  (pset.get< double >("RadiationLength" ));
  this->SetAtomicNumber     (pset.get< double >("AtomicNumber"));
  this->SetAtomicMass       (pset.get< double >("AtomicMass"));
  this->SetMeanExcitationEnergy (pset.get< double >("ExcitationEnergy"));

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
  << " different sizes - " << fSlowScintSpectrum.size()
  << " " << fSlowScintEnergies.size();
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
