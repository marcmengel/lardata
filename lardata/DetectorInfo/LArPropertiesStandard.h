/**
 * @file    LArPropertiesStandard.h
 * @brief   Service provider with utility LAr functions
 * @see     LArPropertiesStandard.cxx LArPropertiesStandardTestHelpers.h
 * 
 * The provider detinfo::LArProperiesStandard supports simple setup for testing
 * environment, by including in your test:
 *     
 *     #include "lardata/DetectorInfo/LArPropertiesStandardTestHelpers.h"
 *     
 */
////////////////////////////////////////////////////////////////////////
// Historical authors:
//
// maddalena.antonello@lngs.infn.it
// ornella.palamara@lngs.infn.it
// msoderbe@syr.edu
// joshua.spitz@yale.edu
//
// Optical Properties:
// bjpjones@mit.edu
//
// Separation of service from Detector info class:
// jpaley@fnal.gov
// 
// Test system:
// petrillo@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#ifndef DETECTORINFO_LARPROPERTIESSTANDARD_H
#define DETECTORINFO_LARPROPERTIESSTANDARD_H

//
// This flag is for temporary code to work around the missing OptionalAtom
// feature in older fhiclcpp
//
#define DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM 0

// LArSoft libraries
#include "lardata/DetectorInfo/LArProperties.h"

// FHiCL libraries
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h"
#if DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM
#	include "fhiclcpp/types/OptionalAtom.h"
#endif
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"

// C/C++ standard libraries
#include <string>
#include <vector>
#include <map>
#include <set>





namespace detinfo {
  /**
   * @brief Properties related to liquid argon environment in the detector
   *
   * This class can access databases via DatabaseUtil service.
   * 
   * @note Some of the database connection properties are established before
   * the beginning of the job and if they change this service will not be
   * aware of it. These properties petrain, so far, only the connection mode
   * and not any content of the databases themselves.
   * @note 2: the database connection features for this base class have been removed
   */
  class LArPropertiesStandard : public LArProperties {
      public:
    LArPropertiesStandard();
    explicit LArPropertiesStandard
      (fhicl::ParameterSet const& pset, std::set<std::string> ignore_params = {});
    LArPropertiesStandard(LArPropertiesStandard const&) = delete;
    virtual ~LArPropertiesStandard() = default;
    
    /**
     * @brief Configures the provider
     * @param p configuration parameter set
     * @param ignore_params parameters to be ignored (optional)
     * 
     * This method will validate the parameter set (except for the parameters
     * it's explicitly told to ignore) and extract the useful information out
     * of it.
     */
    bool   Configure
      (fhicl::ParameterSet const& pset, std::set<std::string> ignore_params = {});
    bool   Update(uint64_t ts=0);
    
    virtual double RadiationLength()  	     const override { return fRadiationLength; } ///< g/cm^2      
    
    virtual double Argon39DecayRate()              const override { return fArgon39DecayRate; }  // decays per cm^3 per second

    /// Ar atomic number
    virtual double AtomicNumber() const override { return fZ; }
    /// Ar atomic mass (g/mol)
    virtual double AtomicMass() const override { return fA; }
    /// Ar mean excitation energy (eV)
    virtual double ExcitationEnergy() const override { return fI; }
    
    virtual double ScintResolutionScale() const override { return fScintResolutionScale; }
    virtual double ScintFastTimeConst()   const override { return fScintFastTimeConst;   } 
    virtual double ScintSlowTimeConst()   const override { return fScintSlowTimeConst;   }
    virtual double ScintBirksConstant()   const override { return fScintBirksConstant;   }
    virtual bool ExtraMatProperties() const { return fExtraMatProperties; }
    virtual double TpbTimeConstant()  const { return fTpbTimeConstant;     }
    virtual bool SimpleBoundary()     const { return fSimpleBoundary;      }
    virtual bool SimpleScint()    const { return fSimpleScint;      }    
    virtual bool ScintByParticleType()    const override { return fScintByParticleType;  }

    virtual double ScintYield(bool prescale = false)         const override { return fScintYield * ScintPreScale(prescale);}
    virtual double ScintPreScale(bool prescale = true)       const override { return (prescale ? fScintPreScale : 1);      }
    virtual double ScintYieldRatio()                         const override { return fScintYieldRatio;                     }
	
    virtual double ProtonScintYield(bool prescale = false)   const override { return fProtonScintYield * ScintPreScale(prescale);  }
    virtual double ProtonScintYieldRatio()                   const override { return fProtonScintYieldRatio;                       }
    virtual double MuonScintYield(bool prescale = false)     const override { return fMuonScintYield * ScintPreScale(prescale);    }
    virtual double MuonScintYieldRatio()                     const override { return fMuonScintYieldRatio;                         }
    virtual double KaonScintYield(bool prescale = false)     const override { return fKaonScintYield * ScintPreScale(prescale);    }
    virtual double KaonScintYieldRatio()                     const override { return fKaonScintYieldRatio;                         }
    virtual double PionScintYield(bool prescale = false)     const override { return fPionScintYield * ScintPreScale(prescale);    }
    virtual double PionScintYieldRatio()                     const override { return fPionScintYieldRatio;                         }
    virtual double ElectronScintYield(bool prescale = false) const override { return fElectronScintYield * ScintPreScale(prescale);}
    virtual double ElectronScintYieldRatio()                 const override { return fElectronScintYieldRatio;                     }
    virtual double AlphaScintYield(bool prescale = false)    const override { return fAlphaScintYield * ScintPreScale(prescale);   }
    virtual double AlphaScintYieldRatio()                    const override { return fAlphaScintYieldRatio;                        }
    virtual bool CerenkovLightEnabled()                      const override { return fEnableCerenkovLight;                         }
	
	
    virtual std::map<double, double> SlowScintSpectrum() const override;   
    virtual std::map<double, double> FastScintSpectrum() const override;
    virtual std::map<double, double> RIndexSpectrum() const override;
    virtual std::map<double, double> AbsLengthSpectrum() const override;
    virtual std::map<double, double> RayleighSpectrum() const override;
    virtual std::map<double, double>  TpbAbs() const override;   
    virtual std::map<double, double>  TpbEm() const override;
    virtual std::map<std::string, std::map<double, double> > SurfaceReflectances() const override;
    virtual std::map<std::string, std::map<double, double> > SurfaceReflectanceDiffuseFractions() const override;
    virtual std::map<std::string, std::map<double, double> > SurfaceTpbReflectances()const override;
    virtual std::map<std::string, std::map<double, double> > SurfaceReflectanceTpbDiffuseFractions()const override;
    double BirksCorrection(double dQdX) const;
    double ModBoxCorrection(double dQdX) const;
    void SetRadiationLength(double rl) { fRadiationLength = rl; }
    void SetElectronlifetime(double el) { fElectronlifetime = el; }

    void SetArgon39DecayRate(double r) { fArgon39DecayRate = r;}
    void SetAtomicNumber(double z) { fZ = z;}
    void SetAtomicMass(double a) { fA = a;}
    void SetMeanExcitationEnergy(double e) { fI = e;}
    void SetEfield(std::vector<double> s) { fEfield = s;}
    void SetFastScintSpectrum(std::vector<double> s) { fFastScintSpectrum = s;}
    void SetFastScintEnergies(std::vector<double> s) { fFastScintEnergies = s;}
    void SetSlowScintSpectrum(std::vector<double> s) { fSlowScintSpectrum = s;}
    void SetSlowScintEnergies(std::vector<double> s) { fSlowScintEnergies = s;}

    void SetTpbEmmisionEnergies(std::vector<double> s) { fTpbEmmisionEnergies = s;}
    void SetTpbEmmisionSpectrum(std::vector<double> s) { fTpbEmmisionSpectrum = s;}
    void SetTpbAbsorptionEnergies(std::vector<double> s) { fTpbAbsorptionEnergies = s;}
    void SetTpbAbsorptionSpectrum(std::vector<double> s) { fTpbAbsorptionSpectrum = s;}

    void SetRIndexSpectrum(std::vector<double> s)    { fRIndexSpectrum = s;}
    void SetRIndexEnergies(std::vector<double> s)    { fRIndexEnergies = s;}
    void SetAbsLengthSpectrum(std::vector<double> s) { fAbsLengthSpectrum = s;}
    void SetAbsLengthEnergies(std::vector<double> s) { fAbsLengthEnergies = s;}
    void SetRayleighSpectrum(std::vector<double> s)  { fRayleighSpectrum = s;}
    void SetRayleighEnergies(std::vector<double> s)  { fRayleighEnergies = s;}

    void SetScintByParticleType(bool l)        { fScintByParticleType = l;}
    void SetSimpleScint(bool l)        { fSimpleScint = l;}
    void SetExtraMatProperties(bool l)        { fExtraMatProperties = l;}
    void SetSimpleBoundary(bool l)        { fSimpleBoundary = l;}
    void SetTpbTimeConstant(double y)         { fTpbTimeConstant = y;}
    void SetProtonScintYield(double y)         { fProtonScintYield = y;}
    void SetProtonScintYieldRatio(double r)    { fProtonScintYieldRatio = r;}
    void SetMuonScintYield(double y)           { fMuonScintYield = y;}
    void SetMuonScintYieldRatio(double r)      { fMuonScintYieldRatio = r;}
    void SetPionScintYield(double y)           { fPionScintYield = y;}
    void SetPionScintYieldRatio(double r)      { fPionScintYieldRatio = r;}
    void SetKaonScintYield(double y)           { fKaonScintYield = y;}
    void SetKaonScintYieldRatio(double r)      { fKaonScintYieldRatio = r;}
    void SetElectronScintYield(double y)       { fElectronScintYield = y;}
    void SetElectronScintYieldRatio(double r)  { fElectronScintYieldRatio = r;}
    void SetAlphaScintYield(double y)          { fAlphaScintYield = y;}
    void SetAlphaScintYieldRatio(double r)     { fAlphaScintYieldRatio = r;}

    void SetScintYield(double y)               { fScintYield = y;}
    void SetScintPreScale(double s)            { fScintPreScale = s;}
    void SetScintResolutionScale(double r)     { fScintResolutionScale = r; }
    void SetScintFastTimeConst(double t)       { fScintFastTimeConst = t;}
    void SetScintSlowTimeConst(double t)       { fScintSlowTimeConst = t;}
    void SetScintYieldRatio(double r)          { fScintYieldRatio = r;}
    void SetScintBirksConstant(double kb)      { fScintBirksConstant = kb;}
    void SetTemperature(double te)      { fTemperature = te;}
    void SetEnableCerenkovLight(bool f)        { fEnableCerenkovLight = f; }

    void SetReflectiveSurfaceNames(std::vector<std::string> n) { fReflectiveSurfaceNames = n;}
    void SetReflectiveSurfaceEnergies(std::vector<double> e)   { fReflectiveSurfaceEnergies = e;}
    void SetReflectiveSurfaceReflectances(std::vector<std::vector<double> > r) { fReflectiveSurfaceReflectances = r;}
    void SetReflectiveSurfaceDiffuseFractions(std::vector<std::vector<double> > f) { fReflectiveSurfaceDiffuseFractions = f;}

    void SetReflectiveSurfaceTpbNames(std::vector<std::string> tn) { fReflectiveSurfaceTpbNames = tn;}
    void SetReflectiveSurfaceTpbEnergies(std::vector<double> ten)   { fReflectiveSurfaceTpbEnergies = ten;}
    void SetReflectiveSurfaceTpbReflectances(std::vector<std::vector<double> > tr) { fReflectiveSurfaceTpbReflectances = tr;}
    void SetReflectiveSurfaceTpbDiffuseFractions(std::vector<std::vector<double> > tf) { fReflectiveSurfaceTpbDiffuseFractions = tf;}


  private:
  protected:
    
    /// structure with all configuration parameters
    struct Configuration_t {
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;
      
      fhicl::Atom<double> RadiationLength
        { Name("RadiationLength" ), Comment("radiation length [g/cm^2]") };
      fhicl::Atom<double> AtomicNumber
        { Name("AtomicNumber"    ), Comment("atomic number (yes, yes, it's 18...)") };
      fhicl::Atom<double> AtomicMass
        { Name("AtomicMass"      ), Comment("atomic mass [g/mol]") };
      fhicl::Atom<double> Electronlifetime
        { Name("Electronlifetime"      ), Comment("Electronlifetime") };

 
      fhicl::Atom<double> MeanExcitationEnergy
        { Name("ExcitationEnergy"), Comment("mean excitation energy [eV]") };
      
      fhicl::Atom<double> Argon39DecayRate
        { Name("Argon39DecayRate"), Comment("decays/(cm^3 s)") };
      fhicl::Sequence<double> Efield { Name("Efield"), Comment("") };      
      // scintillation parameters
      fhicl::Sequence<double> FastScintEnergies { Name("FastScintEnergies"), Comment("") };
      fhicl::Sequence<double> FastScintSpectrum { Name("FastScintSpectrum"), Comment("") };
      fhicl::Sequence<double> SlowScintEnergies { Name("SlowScintEnergies"), Comment("") };
      fhicl::Sequence<double> SlowScintSpectrum { Name("SlowScintSpectrum"), Comment("") };
      fhicl::Sequence<double> TpbEmmisionEnergies { Name("TpbEmmisionEnergies"), Comment("") };
      fhicl::Sequence<double> TpbEmmisionSpectrum { Name("TpbEmmisionSpectrum"), Comment("") };
      fhicl::Sequence<double> TpbAbsorptionEnergies { Name("TpbAbsorptionEnergies"), Comment("") };
      fhicl::Sequence<double> TpbAbsorptionSpectrum { Name("TpbAbsorptionSpectrum"), Comment("") };
      fhicl::Sequence<double> AbsLengthEnergies { Name("AbsLengthEnergies"), Comment("") };
      fhicl::Sequence<double> AbsLengthSpectrum { Name("AbsLengthSpectrum"), Comment("") };
      fhicl::Sequence<double> RIndexEnergies    { Name("RIndexEnergies"   ), Comment("") };
      fhicl::Sequence<double> RIndexSpectrum    { Name("RIndexSpectrum"   ), Comment("") };
      fhicl::Sequence<double> RayleighEnergies  { Name("RayleighEnergies" ), Comment("") };
      fhicl::Sequence<double> RayleighSpectrum  { Name("RayleighSpectrum" ), Comment("") };
      
      fhicl::Atom<double> ScintResolutionScale { Name("ScintResolutionScale"), Comment("") };
      fhicl::Atom<double> ScintFastTimeConst   { Name("ScintFastTimeConst"  ), Comment("") };
      fhicl::Atom<double> ScintSlowTimeConst   { Name("ScintSlowTimeConst"  ), Comment("") };
      fhicl::Atom<double> ScintBirksConstant   { Name("ScintBirksConstant"  ), Comment("") };
      fhicl::Atom<double> ScintYield           { Name("ScintYield"          ), Comment("") };
      fhicl::Atom<double> ScintPreScale        { Name("ScintPreScale"       ), Comment("") };
      fhicl::Atom<double> ScintYieldRatio      { Name("ScintYieldRatio"     ), Comment("") };
      fhicl::Atom<double> TpbTimeConstant      { Name("TpbTimeConstant"     ), Comment("") };
      fhicl::Atom<bool  > ScintByParticleType  { Name("ScintByParticleType" ), Comment("") };
      fhicl::Atom<bool  > SimpleScint  { Name("SimpleScintillation" ), Comment("") };
      fhicl::Atom<bool  > SimpleBoundary  { Name("SimpleBoundaryProcess" ), Comment("") };
      fhicl::Atom<bool  > ExtraMatProperties  { Name("LoadExtraMatProperties" ), Comment("") };

#if DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM
      fhicl::OptionalAtom<double> ProtonScintYield
        { Name("ProtonScintYield"       ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> ProtonScintYieldRatio
        { Name("ProtonScintYieldRatio"  ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> MuonScintYield
        { Name("MuonScintYield"         ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> MuonScintYieldRatio
        { Name("MuonScintYieldRatio"    ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> PionScintYield
        { Name("PionScintYield"         ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> PionScintYieldRatio
        { Name("PionScintYieldRatio"    ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> KaonScintYield
        { Name("KaonScintYield"         ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> KaonScintYieldRatio
        { Name("KaonScintYieldRatio"    ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> ElectronScintYield
        { Name("ElectronScintYield"     ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> ElectronScintYieldRatio
        { Name("ElectronScintYieldRatio"), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> AlphaScintYield
        { Name("AlphaScintYield"        ), Comment("(only if ScintByParticleType is true)") };
      fhicl::OptionalAtom<double> AlphaScintYieldRatio
        { Name("AlphaScintYieldRatio"   ), Comment("(only if ScintByParticleType is true)") };
#endif // DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM?
      
      fhicl::Atom<bool  > EnableCerenkovLight  { Name("EnableCerenkovLight" ), Comment("") };
      fhicl::Atom<double> Temperature  { Name("Temperature"   ), Comment("Temperature") };
      fhicl::Sequence<std::string> ReflectiveSurfaceNames
        { Name("ReflectiveSurfaceNames"),            Comment("") };
      fhicl::Sequence<double> ReflectiveSurfaceEnergies
        { Name("ReflectiveSurfaceEnergies"),         Comment("") };
      fhicl::Sequence<fhicl::Sequence<double>> ReflectiveSurfaceReflectances
        { Name("ReflectiveSurfaceReflectances"),     Comment("") };
      fhicl::Sequence<fhicl::Sequence<double>> ReflectiveSurfaceDiffuseFractions
        { Name("ReflectiveSurfaceDiffuseFractions"), Comment("") };

      fhicl::Sequence<std::string> ReflectiveSurfaceTpbNames
        { Name("ReflectiveSurfaceTpbNames"),            Comment("") };
      fhicl::Sequence<double> ReflectiveSurfaceTpbEnergies
        { Name("ReflectiveSurfaceTpbEnergies"),         Comment("") };
      fhicl::Sequence<fhicl::Sequence<double>> ReflectiveSurfaceTpbReflectances
        { Name("ReflectiveSurfaceTpbReflectances"),     Comment("") };
      fhicl::Sequence<fhicl::Sequence<double>> ReflectiveSurfaceTpbDiffuseFractions
        { Name("ReflectiveSurfaceTpbDiffuseFractions"), Comment("") };
      
    }; // Configuration_t
    
#if !DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM
    /// structure with all configuration parameters
    struct ConfigWithScintByType_t: public Configuration_t {
      
      fhicl::Atom<double> ProtonScintYield
        { Name("ProtonScintYield"       ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> ProtonScintYieldRatio
        { Name("ProtonScintYieldRatio"  ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> MuonScintYield
        { Name("MuonScintYield"         ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> MuonScintYieldRatio
        { Name("MuonScintYieldRatio"    ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> PionScintYield
        { Name("PionScintYield"         ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> PionScintYieldRatio
        { Name("PionScintYieldRatio"    ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> KaonScintYield
        { Name("KaonScintYield"         ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> KaonScintYieldRatio
        { Name("KaonScintYieldRatio"    ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> ElectronScintYield
        { Name("ElectronScintYield"     ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> ElectronScintYieldRatio
        { Name("ElectronScintYieldRatio"), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> AlphaScintYield
        { Name("AlphaScintYield"        ), Comment("(only if ScintByParticleType is true)") };
      fhicl::Atom<double> AlphaScintYieldRatio
        { Name("AlphaScintYieldRatio"   ), Comment("(only if ScintByParticleType is true)") };
      
    }; // ConfigWithScintByType_t
#endif // !DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM?
    
    
    bool fIsConfigured;
      
    double                         fRadiationLength;  ///< g/cm^2
    double                         fArgon39DecayRate; ///<  decays per cm^3 per second
      
    // Following parameters are for use in Bethe-Bloch formula for dE/dx.

    double fZ;                ///< Ar atomic number
    double fA;                ///< Ar atomic mass (g/mol)
    double fI;                ///< Ar mean excitation energy (eV)


    // Optical parameters for LAr

    std::vector<double> fFastScintSpectrum;
    std::vector<double> fEfield;
    std::vector<double> fFastScintEnergies;
    std::vector<double> fSlowScintSpectrum;
    std::vector<double> fSlowScintEnergies;
    std::vector<double> fRIndexSpectrum;
    std::vector<double> fRIndexEnergies;
    std::vector<double> fAbsLengthSpectrum;
    std::vector<double> fAbsLengthEnergies;
    std::vector<double> fRayleighSpectrum;
    std::vector<double> fRayleighEnergies;
    std::vector<double>               fTpbEmmisionEnergies;
    std::vector<double>               fTpbEmmisionSpectrum;
    std::vector<double>               fTpbAbsorptionEnergies;
    std::vector<double>               fTpbAbsorptionSpectrum;
    std::vector<double>               fReflectiveSurfaceTpbEnergies;
    std::vector<std::vector<double> > fReflectiveSurfaceTpbReflectances;
    std::vector<std::vector<double> > fReflectiveSurfaceTpbDiffuseFractions;
    bool fExtraMatProperties;
    bool fSimpleBoundary;
    bool fScintByParticleType;
    bool fSimpleScint;
    double fProtonScintYield;
    double fProtonScintYieldRatio; 
    double fMuonScintYield;
    double fMuonScintYieldRatio; 
    double fPionScintYield;
    double fPionScintYieldRatio; 
    double fKaonScintYield;
    double fKaonScintYieldRatio; 
    double fElectronScintYield;
    double fElectronScintYieldRatio; 
    double fAlphaScintYield;
    double fAlphaScintYieldRatio; 
    double fElectronlifetime;
    double fTpbTimeConstant;

    double fScintYield;
    double fScintPreScale;
    double fScintResolutionScale;
    double fScintFastTimeConst;
    double fScintSlowTimeConst;
    double fScintYieldRatio;
    double fScintBirksConstant;
    double fTemperature;
    bool fEnableCerenkovLight;
    std::vector<std::string>          fReflectiveSurfaceTpbNames;
    std::vector<std::string>          fReflectiveSurfaceNames;
    std::vector<double>               fReflectiveSurfaceEnergies;
    std::vector<std::vector<double> > fReflectiveSurfaceReflectances;
    std::vector<std::vector<double> > fReflectiveSurfaceDiffuseFractions;
    /*      
	    struct DBsettingsClass {
	    DBsettingsClass();
        
	    bool ToughErrorTreatment; ///< equivalent parameter in DatabaseUtil
	    bool ShouldConnect; ///< equivalent parameter in DatabaseUtil
	    }; // DBsettingsClass
      
	    DBsettingsClass DBsettings; ///< settings read from DB access
    */
    
  public:
    // expose the configuration object for framework service
#if DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM
    using ConfigurationParameters_t = Configuration_t;
#else // !DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM?
    using ConfigurationParameters_t = ConfigWithScintByType_t;
#endif // !DETECTORINFO_LARPROPERTIESSTANDARD_HASOPTIONALATOM?
    
  }; // class LArPropertiesStandard
} //namespace detinfo
#endif // LARPROPERTIES_H
