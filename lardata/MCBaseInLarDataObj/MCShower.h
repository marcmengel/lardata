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

// LArSoft libraries
#include "lardataobj/MCBase/MCStep.h"

// Supporting tools
#include "nusimdata/SimulationBase/MCTruth.h"

// ROOT libraries
#include <TVector3.h>

// Standard C/C++ libraries
#include <string>
#include <vector>
#ifndef __GCCXML__
#  include <utility> // std::move()
#endif // __GCCXML__

namespace sim {

  class MCShower {
    
  public:

    /// Default constructor; clears the local data
    MCShower() { ClearData(); }

    /// Default destructor
    virtual ~MCShower();

#ifndef __GCCXML__

    /// Clear method
    virtual void Clear() { ClearData(); }

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

    double Charge(size_t plane) const;
    double dQdx(size_t plane) const;

    const std::vector<double>& Charge() const { return fPlaneCharge; } 
    const std::vector<double>& dQdx() const { return fdQdx; }


    double dEdx() const { return fdEdx; }
    const TVector3& StartDir() const {return fStartDir;}

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

    /// Copies the specified charge vector (one entry per plane) into this object
    void Charge (const std::vector<double>& q) { fPlaneCharge = q; }
    /// Moves the specified charge vector (one entry per plane) into this object
    void Charge (std::vector<double>&& q) { fPlaneCharge = std::move(q); }

    /// Copies the specified dQ/dx vector (one entry per plane) into this object
    void dQdx (const std::vector<double>& dqdx) { fdQdx = dqdx; }
    /// Moves the specified dQ/dx vector (one entry per plane) into this object
    void dQdx (std::vector<double>&& dqdx) { fdQdx = std::move(dqdx); }
    
    void dEdx    (double dedx) {fdEdx = dedx;}
    void dEdxRAD (double dedx) {fdEdx_radial = dedx;}
    

    
#endif // __GCCXML__

  protected:

    /// @{
    /// @name Origin info
    simb::Origin_t fOrigin;    ///< Origin information
    /// @}

    /// @{
    /// @name Shower particle info
    int          fPDGCode;   ///< Shower particle PDG code
    unsigned int fTrackID;   ///< Shower particle G4 track ID
    std::string  fProcess;   ///< Shower particle's creation process
    MCStep       fStart;     ///< Shower particle's G4 start point
    MCStep       fEnd;       ///< Shower particle's G4 end point
    TVector3 fStartDir; ///< Shower Starting Direction, within the first 2.4cm
    /// @}


    /// @{
    /// @name Mother's particle info
    int          fMotherPDGCode; ///< Shower's mother PDG code   
    unsigned int fMotherTrackID; ///< Shower's mother G4 track ID
    std::string  fMotherProcess; ///< Shower's mother creation process
    MCStep       fMotherStart;   ///< Shower's mother G4 start point
    MCStep       fMotherEnd;     ///< Shower's mother G4 end point
    /// @}

    /// @{
    /// @name Ancestor's particle info
    int          fAncestorPDGCode; ///< Shower's ancestor PDG code   
    unsigned int fAncestorTrackID; ///< Shower's ancestor G4 track ID
    std::string  fAncestorProcess; ///< Shower's ancestor creation process
    MCStep       fAncestorStart;   ///< Shower's ancestor G4 start point
    MCStep       fAncestorEnd;     ///< Shower's ancestor G4 end point
    /// @}

    /// @{
    /// @name Energy deposition info
    std::vector<unsigned int>  fDaughterTrackID; ///< Daughters' track ID
    MCStep                     fDetProfile;      ///< Combined energy deposition information
    double                     fdEdx;            ///< Shower True dEdx 
    double                     fdEdx_radial;     ///< Shower True dEdx, with a radial requirement  
    /// @}
  

    /// @{
    /// @name Charge per plane
    std::vector<double> fPlaneCharge; ///< Charge deposit per plane
    std::vector<double> fdQdx; ///< Charge deposit per plane
    /// @}
    
    
    /// Clears the fields of this class
    void ClearData();
    
  }; // class MCShower

} // namespace sim

#endif // MCSHOWER_H
/** @} */ // end of doxygen group 
