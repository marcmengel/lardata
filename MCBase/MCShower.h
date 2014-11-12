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

    simb::Origin_t     Origin    () const { return fOrigin;    }

    int                PdgCode   () const { return fPDGCode;   }
    unsigned int       G4TrackID () const { return fG4TrackID; } 
    const std::string& Process   () const { return fProcess;   }
    const MCStep&      G4Start   () const { return fG4Start;   }
    const MCStep&      G4End     () const { return fG4End;     }

    int                MotherPdgCode   () const { return fMotherPDGCode;   }
    unsigned int       MotherG4TrackID () const { return fMotherG4TrackID; }
    const std::string& MotherProcess   () const { return fMotherProcess;   }
    const MCStep&      MotherG4Start   () const { return fMotherG4Start;   }
    const MCStep&      MotherG4End     () const { return fMotherG4End;     }

    int                AncestorPdgCode   () const { return fAncestorPDGCode;   }
    unsigned int       AncestorG4TrackID () const { return fAncestorG4TrackID; }
    const std::string& AncestorProcess   () const { return fAncestorProcess;   }
    const MCStep&      AncestorG4Start   () const { return fAncestorG4Start;   }
    const MCStep&      AncestorG4End     () const { return fAncestorG4End;     }

    const MCStep& DetProfile () const { return fDetProfile; }
    
    const std::vector<unsigned int>&  DaughterTrackID() const { return fDaughterTrackID; }

    double Charge(const size_t plane) const;

    //--- Setters ---//
    void Origin    ( simb::Origin_t o ) { fOrigin    = o;    }

    void PdgCode   ( int id                  ) { fPDGCode   = id;   }
    void G4TrackID ( unsigned int id         ) { fG4TrackID = id;   }
    void Process   ( const std::string &name ) { fProcess   = name; }
    void G4Start   ( const MCStep &s         ) { fG4Start   = s;    }
    void G4End     ( const MCStep &s         ) { fG4End     = s;    }

    void MotherPdgCode   ( int id                  ) { fMotherPDGCode   = id;   }
    void MotherG4TrackID ( unsigned int id         ) { fMotherG4TrackID = id;   }
    void MotherProcess   ( const std::string& name ) { fMotherProcess   = name; }
    void MotherG4Start   ( const MCStep& s         ) { fMotherG4Start   = s;    }
    void MotherG4End     ( const MCStep& s         ) { fMotherG4End     = s;    }

    void AncestorPdgCode   ( int id                  ) { fAncestorPDGCode   = id;   }
    void AncestorG4TrackID ( unsigned int id         ) { fAncestorG4TrackID = id;   }
    void AncestorProcess   ( const std::string& name ) { fAncestorProcess   = name; }
    void AncestorG4Start   ( const MCStep& s         ) { fAncestorG4Start   = s;    }
    void AncestorG4End     ( const MCStep& s         ) { fAncestorG4End     = s;    }

    void DetProfile ( const MCStep& s) { fDetProfile = s; }

    void DaughterTrackID ( const std::vector<unsigned int>& id_v ) { fDaughterTrackID = id_v; }

    void Charge (const std::vector<double>& q) { fPlaneCharge = q; }

#endif

  protected:

    //---- Origin info ----//
    simb::Origin_t fOrigin;    ///< Origin information

    //---- Shower particle info ----//
    int          fPDGCode;     ///< Shower particle PDG code
    unsigned int fG4TrackID;   ///< Shower particle G4 track ID
    std::string  fProcess;     ///< Shower particle's creation process
    MCStep       fG4Start;     ///< Shower particle's G4 start point
    MCStep       fG4End;       ///< Shower particle's G4 end point

    //---- Mother's particle info ---//
    int          fMotherPDGCode;   ///< Shower's mother PDG code   
    unsigned int fMotherG4TrackID; ///< Shower's mother G4 track ID
    std::string  fMotherProcess;   ///< Shower's mother creation process
    MCStep       fMotherG4Start;   ///< Shower's mother G4 start point
    MCStep       fMotherG4End;     ///< Shower's mother G4 end point

    //---- Ancestor's particle info ---//
    int          fAncestorPDGCode;   ///< Shower's ancestor PDG code   
    unsigned int fAncestorG4TrackID; ///< Shower's ancestor G4 track ID
    std::string  fAncestorProcess;   ///< Shower's ancestor creation process
    MCStep       fAncestorG4Start;   ///< Shower's ancestor G4 start point
    MCStep       fAncestorG4End;     ///< Shower's ancestor G4 end point

    //---- Energy deposition info ----//
    std::vector<unsigned int>  fDaughterTrackID; ///< Daughters' track ID
    MCStep                     fDetProfile;      ///< Combined energy deposition information

    //---- Charge per plane ----//
    std::vector<double> fPlaneCharge; ///< Charge deposit per plane
  };

}

#endif
/** @} */ // end of doxygen group 
