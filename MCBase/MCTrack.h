/**
 * \file MCTrack.h
 *
 * \ingroup MCBase
 * 
 * \brief Class def header for mctrack data container
 *
 * @author Kazu - Nevis 2014
 */

/** \addtogroup MCBase

    @{*/

#ifndef MCTRACK_H
#define MCTRACK_H

#include <vector>
#include "MCStep.h"
#include "SimulationBase/MCTruth.h"

namespace sim{

  /**
     \class MCTrack
  */
  class MCTrack : public std::vector<sim::MCStep> {
    
  public:
    
    /// Default constructor
    MCTrack() : std::vector<sim::MCStep>() {Clear();}
    
    /// Default destructor
    virtual ~MCTrack(){}

    void Clear();

    #ifndef __GCCXML__

    simb::Origin_t      Origin     () const { return fOrigin;            }
    int                 PdgCode    () const { return fPDGCode;           }
    unsigned int        G4TrackID  () const { return fG4TrackID;         }
    const std::string&  Process    () const { return fProcess;           }
    const MCStep&       G4Start    () const { return fG4Start;           }
    const MCStep&       G4End      () const { return fG4End;             }

    int                MotherPdgCode   () const { return fMotherPDGCode;     }
    unsigned int       MotherG4TrackID () const { return fMotherG4TrackID;   }
    const std::string& MotherProcess   () const { return fMotherProcess;     }
    const MCStep&      MotherG4Start   () const { return fMotherG4Start;     }
    const MCStep&      MotherG4End     () const { return fMotherG4End;       }

    int                AncestorPdgCode   () const { return fAncestorPDGCode;   }
    unsigned int       AncestorG4TrackID () const { return fAncestorG4TrackID; }
    const std::string& AncestorProcess   () const { return fMotherProcess;     }
    const MCStep&      AncestorG4Start   () const { return fAncestorG4Start;   }
    const MCStep&      AncestorG4End     () const { return fAncestorG4End;     }

    void Origin          ( simb::Origin_t o ) { fOrigin    = o;        }
    void PdgCode         ( int id           ) { fPDGCode   = id;       }
    void G4TrackID       ( unsigned int id  ) { fG4TrackID = id;       }
    void Process         ( std::string name ) { fProcess   = name;     }
    void G4Start         ( const MCStep s   ) { fG4Start   = s;        }
    void G4End           ( const MCStep s   ) { fG4End     = s;        }

    void MotherPdgCode   ( int id               ) { fMotherPDGCode   = id; }
    void MotherG4TrackID ( unsigned int id      ) { fMotherG4TrackID = id; }
    void MotherProcess   ( const std::string& n ) { fMotherProcess   = n;  }
    void MotherG4Start   ( const MCStep& s      ) { fMotherG4Start   = s;  }
    void MotherG4End     ( const MCStep& s      ) { fMotherG4End     = s;  }

    void AncestorPdgCode   ( int id               ) { fAncestorPDGCode   = id; }
    void AncestorG4TrackID ( unsigned int id      ) { fAncestorG4TrackID = id; }
    void AncestorProcess   ( const std::string& n ) { fAncestorProcess   = n;  }
    void AncestorG4Start   ( const MCStep& s      ) { fAncestorG4Start   = s;  }
    void AncestorG4End     ( const MCStep& s      ) { fAncestorG4End     = s;  }

    #endif

  protected:

    simb::Origin_t fOrigin;    ///< Origin of this particle (see simb::Origin_t)
    int            fPDGCode;   ///< PDG code of this track particle
    unsigned int   fG4TrackID; ///< G4 track ID
    std::string    fProcess;   ///< Creation process of this track particle
    MCStep         fG4Start;   ///< G4 start position/momentum of this track particle
    MCStep         fG4End;     ///< G4 end position/momentum of this track particle

    int            fMotherPDGCode;   ///< This particle's mother's PDG code
    unsigned int   fMotherG4TrackID; ///< This particle's mother's G4 track ID
    std::string    fMotherProcess;   ///< This particle's mother's process name
    MCStep         fMotherG4Start;   ///< This particle's mother's start position/momentum
    MCStep         fMotherG4End;     ///< This particle's mother's end position/momentum

    int            fAncestorPDGCode;   ///< This particle's ancestor's PDG code
    unsigned int   fAncestorG4TrackID; ///< This particle's ancestor's G4 track ID
    std::string    fAncestorProcess;   ///< This particle's ancestor's process name
    MCStep         fAncestorG4Start;   ///< This particle's ancestor's start position/momentum
    MCStep         fAncestorG4End;     ///< This particle's ancestor's start position/momentum
  };
}
#endif

/** @} */ // end of doxygen group 
