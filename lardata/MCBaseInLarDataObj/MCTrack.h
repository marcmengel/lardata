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
#include "nusimdata/SimulationBase/MCTruth.h"

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

    simb::Origin_t      Origin   () const { return fOrigin;   }
    int                 PdgCode  () const { return fPDGCode;  }
    unsigned int        TrackID  () const { return fTrackID;  }
    const std::string&  Process  () const { return fProcess;  }
    const MCStep&       Start    () const { return fStart;    }
    const MCStep&       End      () const { return fEnd;      }
    const std::vector< std::vector<double> >& dQdx() const{ return fdQdx;}  // dQdx[# of MCSteps][# of plane]
    const std::vector<double>& dEdx() const {return fdEdx;} // dEdx[# of MCSteps]

    int                MotherPdgCode () const { return fMotherPDGCode; }
    unsigned int       MotherTrackID () const { return fMotherTrackID; }
    const std::string& MotherProcess () const { return fMotherProcess; }
    const MCStep&      MotherStart   () const { return fMotherStart;   }
    const MCStep&      MotherEnd     () const { return fMotherEnd;     }

    int                AncestorPdgCode () const { return fAncestorPDGCode; }
    unsigned int       AncestorTrackID () const { return fAncestorTrackID; }
    const std::string& AncestorProcess () const { return fMotherProcess;   }
    const MCStep&      AncestorStart   () const { return fAncestorStart;   }
    const MCStep&      AncestorEnd     () const { return fAncestorEnd;     }

    void Origin        ( simb::Origin_t o ) { fOrigin  = o;    }
    void PdgCode       ( int id           ) { fPDGCode = id;   }
    void TrackID       ( unsigned int id  ) { fTrackID = id;   }
    void Process       ( std::string name ) { fProcess = name; }
    void Start         ( const MCStep s   ) { fStart   = s;    }
    void End           ( const MCStep s   ) { fEnd     = s;    }
    void dEdx          ( const std::vector<double> s) { fdEdx = s;}
    void dQdx          ( const std::vector<std::vector<double> > s) { fdQdx = s;}


    void MotherPdgCode ( int id               ) { fMotherPDGCode = id; }
    void MotherTrackID ( unsigned int id      ) { fMotherTrackID = id; }
    void MotherProcess ( const std::string& n ) { fMotherProcess = n;  }
    void MotherStart   ( const MCStep& s      ) { fMotherStart   = s;  }
    void MotherEnd     ( const MCStep& s      ) { fMotherEnd     = s;  }

    void AncestorPdgCode ( int id               ) { fAncestorPDGCode = id; }
    void AncestorTrackID ( unsigned int id      ) { fAncestorTrackID = id; }
    void AncestorProcess ( const std::string& n ) { fAncestorProcess = n;  }
    void AncestorStart   ( const MCStep& s      ) { fAncestorStart   = s;  }
    void AncestorEnd     ( const MCStep& s      ) { fAncestorEnd     = s;  }

    #endif

  protected:

    simb::Origin_t fOrigin;  ///< Origin of this particle (see simb::Origin_t)
    int            fPDGCode; ///< PDG code of this track particle
    unsigned int   fTrackID; ///< G4 track ID
    std::string    fProcess; ///< G4 creation process of this track particle
    MCStep         fStart;   ///< G4 start position/momentum of this track particle
    MCStep         fEnd;     ///< G4 end position/momentum of this track particle
    std::vector<std::vector<double> > fdQdx; //< the G4 electron yeild per plane between each step // [N Plane][MCSteps - 1]
    std::vector<double> fdEdx;//< the G4 "ionization" energy loss between each step // [MCSteps - 1]

    int            fMotherPDGCode; ///< This particle's mother's PDG code
    unsigned int   fMotherTrackID; ///< This particle's mother's G4 track ID
    std::string    fMotherProcess; ///< This particle's mother's G4 process name
    MCStep         fMotherStart;   ///< This particle's mother's G4 start position/momentum
    MCStep         fMotherEnd;     ///< This particle's mother's G4 end position/momentum

    int            fAncestorPDGCode; ///< This particle's ancestor's PDG code
    unsigned int   fAncestorTrackID; ///< This particle's ancestor's G4 track ID
    std::string    fAncestorProcess; ///< This particle's ancestor's G4 process name
    MCStep         fAncestorStart;   ///< This particle's ancestor's G4 start position/momentum
    MCStep         fAncestorEnd;     ///< This particle's ancestor's G4 start position/momentum
  };
}
#endif

/** @} */ // end of doxygen group 
