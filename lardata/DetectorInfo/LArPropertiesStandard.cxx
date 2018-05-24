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
#include "lardataalg/DetectorInfo/LArPropertiesStandard.h"
#include "larcorealg/CoreUtils/ProviderUtil.h" // lar::IgnorableProviderConfigKeys()
#include "larcoreobj/SimpleTypesAndConstants/PhysicalConstants.h"

// ROOT includes
#include "TMath.h"
#include "TH1.h"
#include <Rtypes.h>

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/exception.h"
//#include "fhiclcpp/types/detail/validationException.h"
//#include "fhiclcpp/types/detail/validate_ParameterSet.h" // fhiclcpp::detail::fillMissingKeysMsg()

//-----------------------------------------------
detinfo::LArPropertiesStandard::LArPropertiesStandard()
  : fIsConfigured(false)
{
}

//-----------------------------------------------
detinfo::LArPropertiesStandard::LArPropertiesStandard(
  fhicl::ParameterSet const& pset,
  std::set<std::string> ignore_params /* = {} */
): LArPropertiesStandard()
{
  this->Configure(pset, ignore_params);
}

#if 0
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
#endif // 0

//------------------------------------------------
bool detinfo::LArPropertiesStandard::Configure(
  fhicl::ParameterSet const& pset,
  std::set<std::string> ignore_params /* = {} */
) {
  // we need to know whether we require the additional ScintByParticleType parameters:
  const bool bScintByParticleType = pset.get<bool>("ScintByParticleType", false);
  
  std::set<std::string> ignorable_keys = lar::IgnorableProviderConfigKeys();
  ignorable_keys.insert(ignore_params.begin(), ignore_params.end());
  
#if DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM
  // validation happens here:
  fhicl::Table<Configuration_t> config_table { pset, lar::IgnorableProviderConfigKeys() };
  Configuration_t const& config = config_table();
  
  if(bScintByParticleType) {
    double value;
    std::string errmsg;
    if (config.ProtonScintYield(value)) SetProtonScintYield(value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.ProtonScintYield       .key() });
    if (config.ProtonScintYieldRatio  (value)) SetProtonScintYieldRatio  (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.ProtonScintYieldRatio  .key() });
    if (config.MuonScintYield         (value)) SetMuonScintYield         (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.MuonScintYield         .key() });
    if (config.MuonScintYieldRatio    (value)) SetMuonScintYieldRatio    (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.MuonScintYieldRatio    .key() });
    if (config.PionScintYield         (value)) SetPionScintYield         (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.PionScintYield         .key() });
    if (config.PionScintYieldRatio    (value)) SetPionScintYieldRatio    (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.PionScintYieldRatio    .key() });
    if (config.KaonScintYield         (value)) SetKaonScintYield         (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.KaonScintYield         .key() });
    if (config.KaonScintYieldRatio    (value)) SetKaonScintYieldRatio    (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.KaonScintYieldRatio    .key() });
    if (config.ElectronScintYield     (value)) SetElectronScintYield     (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.ElectronScintYield     .key() });
    if (config.ElectronScintYieldRatio(value)) SetElectronScintYieldRatio(value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.ElectronScintYieldRatio.key() });
    if (config.AlphaScintYield        (value)) SetAlphaScintYield        (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.AlphaScintYield        .key() });
    if (config.AlphaScintYieldRatio   (value)) SetAlphaScintYieldRatio   (value);
    else errmsg += fhicl::detail::fillMissingKeysMsg(&config_table, { config.AlphaScintYieldRatio   .key() });
    if (!errmsg.empty()) {
      throw fhicl::detail::validationException((
        "[these parameters are REQUIRED when ScintByParticleType is true; the list may be incomplete]\n"
        + errmsg
        ).c_str());
    } // if missing parameters
  } // if bScintByParticleType
  
  // read parameters
#else // !DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM
  
  if (!bScintByParticleType) { // ignore the following keys
    ConfigWithScintByType_t config; // to get the keys
    ignorable_keys.insert(config.ProtonScintYield       .key());
    ignorable_keys.insert(config.ProtonScintYieldRatio  .key());
    ignorable_keys.insert(config.MuonScintYield         .key());
    ignorable_keys.insert(config.MuonScintYieldRatio    .key());
    ignorable_keys.insert(config.PionScintYield         .key());
    ignorable_keys.insert(config.PionScintYieldRatio    .key());
    ignorable_keys.insert(config.KaonScintYield         .key());
    ignorable_keys.insert(config.KaonScintYieldRatio    .key());
    ignorable_keys.insert(config.ElectronScintYield     .key());
    ignorable_keys.insert(config.ElectronScintYieldRatio.key());
    ignorable_keys.insert(config.AlphaScintYield        .key());
    ignorable_keys.insert(config.AlphaScintYieldRatio   .key());
  } // if !bScintByParticleType
  
  // validation happens here:
  fhicl::Table<ConfigWithScintByType_t> config_table { pset, ignorable_keys };
  
  // read parameters
  ConfigWithScintByType_t const& config = config_table();
  if (bScintByParticleType) {
    SetProtonScintYield       (config.ProtonScintYield       ());
    SetProtonScintYieldRatio  (config.ProtonScintYieldRatio  ());
    SetMuonScintYield         (config.MuonScintYield         ());
    SetMuonScintYieldRatio    (config.MuonScintYieldRatio    ());
    SetPionScintYield         (config.PionScintYield         ());
    SetPionScintYieldRatio    (config.PionScintYieldRatio    ());
    SetKaonScintYield         (config.KaonScintYield         ());
    SetKaonScintYieldRatio    (config.KaonScintYieldRatio    ());
    SetElectronScintYield     (config.ElectronScintYield     ());
    SetElectronScintYieldRatio(config.ElectronScintYieldRatio());
    SetAlphaScintYield        (config.AlphaScintYield        ());
    SetAlphaScintYieldRatio   (config.AlphaScintYieldRatio   ());
  } // if ScintByParticleType
#endif // DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM??
  
  SetRadiationLength      (config.RadiationLength());
  
  SetAtomicNumber         (config.AtomicNumber());
  SetAtomicMass           (config.AtomicMass());
  SetMeanExcitationEnergy (config.MeanExcitationEnergy());

  SetArgon39DecayRate     (config.Argon39DecayRate());

  SetFastScintEnergies(config.FastScintEnergies());
  SetFastScintSpectrum(config.FastScintSpectrum());
  SetSlowScintEnergies(config.SlowScintEnergies());
  SetSlowScintSpectrum(config.SlowScintSpectrum());
  SetAbsLengthEnergies(config.AbsLengthEnergies());
  SetAbsLengthSpectrum(config.AbsLengthSpectrum());
  SetRIndexEnergies   (config.RIndexEnergies   ());
  SetRIndexSpectrum   (config.RIndexSpectrum   ());
  SetRayleighEnergies (config.RayleighEnergies ());
  SetRayleighSpectrum (config.RayleighSpectrum ());

  SetScintResolutionScale(config.ScintResolutionScale());
  SetScintFastTimeConst  (config.ScintFastTimeConst  ());
  SetScintSlowTimeConst  (config.ScintSlowTimeConst  ());
  SetScintBirksConstant  (config.ScintBirksConstant  ());
  SetScintYield          (config.ScintYield          ());
  SetScintPreScale       (config.ScintPreScale       ());
  SetScintYieldRatio     (config.ScintYieldRatio     ());
  SetScintByParticleType (config.ScintByParticleType ());

  SetEnableCerenkovLight(config.EnableCerenkovLight());
  
  SetReflectiveSurfaceNames           (config.ReflectiveSurfaceNames());
  SetReflectiveSurfaceEnergies        (config.ReflectiveSurfaceEnergies());
  SetReflectiveSurfaceReflectances    (config.ReflectiveSurfaceReflectances());
  SetReflectiveSurfaceDiffuseFractions(config.ReflectiveSurfaceDiffuseFractions());

  SetTpbEmmisionEnergies(config.TpbEmmisionEnergies());
  SetTpbEmmisionSpectrum(config.TpbEmmisionSpectrum());
  SetTpbAbsorptionEnergies(config.TpbAbsorptionEnergies());
  SetTpbAbsorptionSpectrum(config.TpbAbsorptionSpectrum());

  SetExtraMatProperties (config.ExtraMatProperties ());  
  SetTpbTimeConstant (config.TpbTimeConstant ());

  
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
//---------------------------------------------------------------------------------
std::map<double, double> detinfo::LArPropertiesStandard::TpbAbs() const
{
  if(fTpbAbsorptionEnergies.size()!=fTpbAbsorptionSpectrum.size()){
    throw cet::exception("Incorrect vector sizes in LArProperties")
      << "The vectors specifying the TpbAbsorption spectrum are "
      << " different sizes - " << fTpbAbsorptionEnergies.size()
      << " " << fTpbAbsorptionSpectrum.size();
  }

  std::map<double, double> ToReturn;
  for(size_t i=0; i!=fTpbAbsorptionSpectrum.size(); ++i)
    ToReturn[fTpbAbsorptionEnergies.at(i)]=fTpbAbsorptionSpectrum.at(i);

  return ToReturn;
}
//--------------------------------------------------------------------------------- 
std::map<double, double> detinfo::LArPropertiesStandard::TpbEm() const
{
  if(fTpbEmmisionEnergies.size()!=fTpbEmmisionSpectrum.size()){
    throw cet::exception("Incorrect vector sizes in LArProperties")
      << "The vectors specifying the TpbEmmision spectrum are "
      << " different sizes - " << fTpbEmmisionEnergies.size()
      << " " << fTpbEmmisionSpectrum.size();
  }
  //using interpolation for more smooth spectrum of TPB emmision - won't affect anything but the effective size of table passed to G4               
  Int_t tablesize=100;
  std::vector<double> new_x;
  double xrange=0.0;
  Double_t *en = new Double_t[int(fTpbEmmisionSpectrum.size())+1];
  Double_t *spectr = new Double_t[int(fTpbEmmisionSpectrum.size())+1];
  for(int j=0;j<int(fTpbEmmisionSpectrum.size())+1;j++){
    if(j==0){
      en[j]=0.;
      en[j]=0.;
    }
    else{
      en[j]=fTpbEmmisionEnergies[j-1];
      spectr[j]=fTpbEmmisionSpectrum[j-1];
      //if(j==int(fTpbEmmisionSpectrum.size())) spectr[j]=+0.5; 
    }
    //std::cout<<j<<" "<<int(fTpbEmmisionSpectrum.size())<<" energiestpb "<<en[j]<<std::endl; 
  }
  TH1D *energyhist=new TH1D();
  energyhist->SetBins(int(fTpbEmmisionSpectrum.size()),en);
  for(int ii=0;ii<int(fTpbEmmisionSpectrum.size());ii++) energyhist->SetBinContent(ii,spectr[ii]);
  xrange=double((en[int(fTpbEmmisionSpectrum.size())]-en[0])/double(fTpbEmmisionSpectrum.size()));
  new_x.clear();
  for(int jj=0; jj<int(tablesize); jj++){

    new_x.push_back(jj*(xrange/double(tablesize)));
    //std::cout<<"position "<<jj<<" "<<new_x[jj]<<" size of table "<<tablesize<<" range x "<<xrange<<std::endl;                        
  }
  std::map<double, double> ToReturn;
  //for(size_t i=0; i!=fTpbEmmisionSpectrum.size(); ++i)        
  //  ToReturn[fTpbEmmisionEnergies.at(i)]=fTpbEmmisionSpectrum.at(i);
  for(int i=0; i<tablesize; i++){
    ToReturn[new_x.at(i)]=energyhist->Interpolate(new_x[i]);
    //std::cout<<ToReturn[new_x[i]]<< " is set in material propertiestpb at energy "<<new_x[i]<<" size of x "<<new_x.size()<<" "<<energyhist->Interpolate(new_x[i])<<std::end;
  }
  delete energyhist;

  delete[] en;
  delete[] spectr;
  return ToReturn;
}
//--------------------------------------------------------------------------------- 
