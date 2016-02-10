////////////////////////////////////////////////////////////////////////
// LArProperties.h
//
// Utility LAr functions
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
////////////////////////////////////////////////////////////////////////
#ifndef LARPROPERTIESSTD_H
#define LARPROPERTIESSTD_H

#include <string>
#include <vector>
#include <map>

#include "DetectorInfo/LArProperties.h"
#include "fhiclcpp/ParameterSet.h"

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
    LArPropertiesStandard(fhicl::ParameterSet const& pset);
    LArPropertiesStandard(LArPropertiesStandard const&) = delete;
    virtual ~LArPropertiesStandard();
    
    bool   Configure(fhicl::ParameterSet const& pset);
    bool   Update(uint64_t ts=0);
    
    virtual double Density(double temperature=0.) const override;                          ///< g/cm^3
    
    virtual double Temperature()                   const override; 				///< kelvin	    
    virtual double RadiationLength()  	     const override { return fRadiationLength; } ///< g/cm^2      
    
    virtual double Argon39DecayRate()              const override { return fArgon39DecayRate; }  // decays per cm^3 per second
    
    /// Restricted mean dE/dx energy loss (MeV/cm).
    virtual double Eloss(double mom, double mass, double tcut) const override;
    
 /// Energy loss fluctuation (sigma_E^2 / length in MeV^2/cm).
    virtual double ElossVar(double mom, double mass) const override;
	
    virtual double ScintResolutionScale() const override { return fScintResolutionScale; }
    virtual double ScintFastTimeConst()   const override { return fScintFastTimeConst;   } 
    virtual double ScintSlowTimeConst()   const override { return fScintSlowTimeConst;   }
    virtual double ScintBirksConstant()   const override { return fScintBirksConstant;   }
	
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
	
    virtual std::map<std::string, std::map<double, double> > SurfaceReflectances() const override;
    virtual std::map<std::string, std::map<double, double> > SurfaceReflectanceDiffuseFractions() const override;
	
    void SetTemperature(double temp) { fTemperature = temp;}
    void SetElectronlifetime(double lt) { fElectronlifetime = lt; }
    void SetRadiationLength(double rl) { fRadiationLength = rl; }
    void SetArgon39DecayRate(double r) { fArgon39DecayRate = r;}
    void SetAtomicNumber(double z) { fZ = z;}
    void SetAtomicMass(double a) { fA = a;}
    void SetMeanExcitationEnergy(double e) { fI = e;}
    void SetSa(double s) { fSa = s;}
    void SetSk(double s) { fSk = s;}
    void SetSx0(double s) { fSx0 = s;}
    void SetSx1(double s) { fSx1 = s;}
    void SetScbar(double s) { fScbar = s;}

    void SetFastScintSpectrum(std::vector<double> s) { fFastScintSpectrum = s;}
    void SetFastScintEnergies(std::vector<double> s) { fFastScintEnergies = s;}
    void SetSlowScintSpectrum(std::vector<double> s) { fSlowScintSpectrum = s;}
    void SetSlowScintEnergies(std::vector<double> s) { fSlowScintSpectrum = s;}
    void SetRIndexSpectrum(std::vector<double> s)    { fRIndexSpectrum = s;}
    void SetRIndexEnergies(std::vector<double> s)    { fRIndexEnergies = s;}
    void SetAbsLengthSpectrum(std::vector<double> s) { fAbsLengthSpectrum = s;}
    void SetAbsLengthEnergies(std::vector<double> s) { fAbsLengthEnergies = s;}
    void SetRayleighSpectrum(std::vector<double> s)  { fRayleighSpectrum = s;}
    void SetRayleighEnergies(std::vector<double> s)  { fRayleighEnergies = s;}

    void SetScintByParticleType(bool l)        { fScintByParticleType = l;}
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
    void SetEnableCerenkovLight(bool f)        { fEnableCerenkovLight = f; }

    void SetReflectiveSurfaceNames(std::vector<std::string> n) { fReflectiveSurfaceNames = n;}
    void SetReflectiveSurfaceEnergies(std::vector<double> e)   { fReflectiveSurfaceEnergies = e;}
    void SetReflectiveSurfaceReflectances(std::vector<std::vector<double> > r) { fReflectiveSurfaceReflectances = r;}
    void SetReflectiveSurfaceDiffuseFractions(std::vector<std::vector<double> > f) { fReflectiveSurfaceDiffuseFractions = f;}

  private:
  protected:

    bool fIsConfigured;
      
    double                         fTemperature;      ///< kelvin
    double                         fElectronlifetime; ///< microseconds
    double                         fRadiationLength;  ///< g/cm^2
    double                         fArgon39DecayRate; ///<  decays per cm^3 per second
      
    // Following parameters are for use in Bethe-Bloch formula for dE/dx.

    double fZ;                ///< Ar atomic number
    double fA;                ///< Ar atomic mass (g/mol)
    double fI;                ///< Ar mean excitation energy (eV)
    double fSa;               ///< Sternheimer parameter a
    double fSk;               ///< Sternheimer parameter k
    double fSx0;              ///< Sternheimer parameter x0
    double fSx1;              ///< Sternheimer parameter x1
    double fScbar;            ///< Sternheimer parameter Cbar


    // Optical parameters for LAr

    std::vector<double> fFastScintSpectrum;
    std::vector<double> fFastScintEnergies;
    std::vector<double> fSlowScintSpectrum;
    std::vector<double> fSlowScintEnergies;
    std::vector<double> fRIndexSpectrum;
    std::vector<double> fRIndexEnergies;
    std::vector<double> fAbsLengthSpectrum;
    std::vector<double> fAbsLengthEnergies;
    std::vector<double> fRayleighSpectrum;
    std::vector<double> fRayleighEnergies;

    bool fScintByParticleType;

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

    double fScintYield;
    double fScintPreScale;
    double fScintResolutionScale;
    double fScintFastTimeConst;
    double fScintSlowTimeConst;
    double fScintYieldRatio;
    double fScintBirksConstant;

    bool fEnableCerenkovLight;

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
    
  }; // class LArPropertiesStandard
} //namespace detinfo
#endif // LARPROPERTIES_H
