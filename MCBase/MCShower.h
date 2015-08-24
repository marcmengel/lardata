/**
 * \file MCShower.h
 *
 * \ingroup MCBase
 * 
 * \brief Class def header for MCShower data container
 *
 * @author Kazu - Nevis 2014
 */

/** \addtogroup MCBase

    @{*/

#ifndef MCSHOWER_H
#define MCSHOWER_H

#include "MCStep.h"
#include <TVector3.h>
#include "SimulationBase/MCTruth.h"

namespace sim {

  class MCShower {
    
  public:

    /// Default constructor
    MCShower() {Clear();}

    /// Default destructor
    virtual ~MCShower(){}

    /// Clear method
    virtual void Clear();

#ifndef __GCCXML__

    //--- Getters ---//

    simb::Origin_t     Origin  () const { return fOrigin;  }

    int                PdgCode () const { return fPDGCode; }
    unsigned int       TrackID () const { return fTrackID; } 
    const std::string& Process () const { return fProcess; }
    const MCStep&      Start   () const { return fStart;   }
    const MCStep&      End     () const { return fEnd;     }

    int                MotherPdgCode () const { return fMotherPDGCode; }
    unsigned int       MotherTrackID () const { return fMotherTrackID; }
    const std::string& MotherProcess () const { return fMotherProcess; }
    const MCStep&      MotherStart   () const { return fMotherStart;   }
    const MCStep&      MotherEnd     () const { return fMotherEnd;     }

    int                AncestorPdgCode () const { return fAncestorPDGCode; }
    unsigned int       AncestorTrackID () const { return fAncestorTrackID; }
    const std::string& AncestorProcess () const { return fAncestorProcess; }
    const MCStep&      AncestorStart   () const { return fAncestorStart;   }
    const MCStep&      AncestorEnd     () const { return fAncestorEnd;     }

    const MCStep& DetProfile () const { return fDetProfile; }
    
    const std::vector<unsigned int>&  DaughterTrackID() const { return fDaughterTrackID; }

    double Charge(const size_t plane) const;

    const double& dEdx() const{ return fdEdx;}

    const TVector3& StartDir() const {return fStartDir;}

    const std::vector<double>& Charge() const { return fPlaneCharge; }


    //--- Setters ---//
    void Origin    ( simb::Origin_t o ) { fOrigin    = o;    }

    void PdgCode  ( int id                    ) { fPDGCode = id;    }
    void TrackID  ( unsigned int id           ) { fTrackID = id;    }
    void Process  ( const std::string &name   ) { fProcess = name;  }
    void Start    ( const MCStep &s           ) { fStart   = s;     }
    void End      ( const MCStep &s           ) { fEnd     = s;     }
    void StartDir ( const TVector3 &sdir) { fStartDir = sdir; }

    void MotherPdgCode ( int id                  ) { fMotherPDGCode = id;   }
    void MotherTrackID ( unsigned int id         ) { fMotherTrackID = id;   }
    void MotherProcess ( const std::string& name ) { fMotherProcess = name; }
    void MotherStart   ( const MCStep& s         ) { fMotherStart   = s;    }
    void MotherEnd     ( const MCStep& s         ) { fMotherEnd     = s;    }

    void AncestorPdgCode ( int id                  ) { fAncestorPDGCode   = id;   }
    void AncestorTrackID ( unsigned int id         ) { fAncestorTrackID = id;     }
    void AncestorProcess ( const std::string& name ) { fAncestorProcess   = name; }
    void AncestorStart   ( const MCStep& s         ) { fAncestorStart   = s;      }
    void AncestorEnd     ( const MCStep& s         ) { fAncestorEnd     = s;      }

    void DetProfile ( const MCStep& s) { fDetProfile = s; }

    void DaughterTrackID ( const std::vector<unsigned int>& id_v ) { fDaughterTrackID = id_v; }

    void Charge (const std::vector<double>& q) { fPlaneCharge = q; }
    
    void dEdx    (const double& dedx) {fdEdx = dedx;}
    void dEdxRAD (const double& dedx) {fdEdx_radial = dedx;}

#endif

  protected:

    //---- Origin info ----//
    simb::Origin_t fOrigin;    ///< Origin information

    //---- Shower particle info ----//
    int          fPDGCode;   ///< Shower particle PDG code
    unsigned int fTrackID;   ///< Shower particle G4 track ID
    std::string  fProcess;   ///< Shower particle's creation process
    MCStep       fStart;     ///< Shower particle's G4 start point
    MCStep       fEnd;       ///< Shower particle's G4 end point
    TVector3 fStartDir; ///< Shower Starting Direction, within the first 2.4cm
  


    //---- Mother's particle info ---//
    int          fMotherPDGCode; ///< Shower's mother PDG code   
    unsigned int fMotherTrackID; ///< Shower's mother G4 track ID
    std::string  fMotherProcess; ///< Shower's mother creation process
    MCStep       fMotherStart;   ///< Shower's mother G4 start point
    MCStep       fMotherEnd;     ///< Shower's mother G4 end point

    //---- Ancestor's particle info ---//
    int          fAncestorPDGCode; ///< Shower's ancestor PDG code   
    unsigned int fAncestorTrackID; ///< Shower's ancestor G4 track ID
    std::string  fAncestorProcess; ///< Shower's ancestor creation process
    MCStep       fAncestorStart;   ///< Shower's ancestor G4 start point
    MCStep       fAncestorEnd;     ///< Shower's ancestor G4 end point

    //---- Energy deposition info ----//
    std::vector<unsigned int>  fDaughterTrackID; ///< Daughters' track ID
    MCStep                     fDetProfile;      ///< Combined energy deposition information
    double                     fdEdx;            ///< Shower True dEdx 
    double                     fdEdx_radial;     ///< Shower True dEdx, with a radial requirement  
  

    //---- Charge per plane ----//
    std::vector<double> fPlaneCharge; ///< Charge deposit per plane
  };

}

#endif
/** @} */ // end of doxygen group 
