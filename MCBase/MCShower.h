#ifndef MCSHOWER_H
#define MCSHOWER_H

#include <vector>
#include "MCLimits.h"
#include "MCBaseException.h"

namespace sim {

  class MCShower {

  public:

    /// Default ctor 
    MCShower() : fMotherVtx        (4,::sim::kINVALID_DOUBLE),
		 fMotherMomentum   (4,::sim::kINVALID_DOUBLE),
		 fDaughterVtx      (4,::sim::kINVALID_DOUBLE),
		 fDaughterMomentum (4,::sim::kINVALID_DOUBLE)
    { Clear(); }

    /// Default dtor
    ~MCShower(){}

    /// Initializer
    void Clear() {

      fMotherPDGID   = ::sim::kINVALID_INT;
      fMotherTrackID = ::sim::kINVALID_UINT;      
      fMotherProcess = "";
      fDaughterTrackID.clear();
      fPlaneCharge.clear();
      for(size_t i=0; i<4; ++i) {

	fMotherVtx[i]        = ::sim::kINVALID_DOUBLE;
	fMotherMomentum[i]   = ::sim::kINVALID_DOUBLE;
	fDaughterVtx[i]      = ::sim::kINVALID_DOUBLE;
	fDaughterMomentum[i] = ::sim::kINVALID_DOUBLE;

      }
    }

#ifndef __GCCXML__

    //
    //--- Getters ---//
    //
    /// Mother PDG code
    const int           MotherPDGID()   const         { return fMotherPDGID;     }
    /// Mother G4 Track ID (to be matched w/ simb::MCParticle::fTrackId
    const unsigned int  MotherTrackID() const         { return fMotherTrackID;   }
    /// Mother G4 creation process string
    const std::string&  MotherCreationProcess() const { return fMotherProcess;   }
    /// Mother creation 4-position (x,y,z,t) [cm,ns]
    const std::vector<double>& MotherPosition() const { return fMotherVtx;       }
    /// Mother initial 4-momentum @ creation vtx (Px, Py, Pz, E) [MeV/c, MeV]
    const std::vector<double>& MotherMomentum() const { return fMotherMomentum;  }
    
    /// Collection G4 Track ID from daughters (to be matched w/ simb::MCParticle::fTrackId)
    const std::vector<unsigned int>& DaughterTrackID() const { return fDaughterTrackID;  }
    /// First daughter (in time) creation 4-position (x,y,z,t) [cm,ns]
    const std::vector<double>& DaughterPosition() const      { return fDaughterVtx;      }
    /// Summed daughter momentum contributed in energy deposition (Px,Py,Pz,E), [MeV/c, MeV]
    const std::vector<double>& DaughterMomentum() const      { return fDaughterMomentum; }
    /// Summed ionization electrons detected per plane
    const double Charge(const unsigned char plane) const
    {
      if(plane >= fPlaneCharge.size())

	throw MCBaseException("Invalid plane ID requested in MCShower::Charge()");

      return fPlaneCharge[plane];
    }
    /// Summed ionization electrons detected per plane (all planes)
    const std::vector<double>& Charge() const { return fPlaneCharge; }

    //
    //--- Setters ---//
    //
    void MotherPDGID(int const id)                              { fMotherPDGID = id;       }
    void MotherTrackID(unsigned int const id)                   { fMotherTrackID = id;     }
    void MotherCreationProcess(std::string const& name)         { fMotherProcess = name;   }
    void MotherPosition(std::vector<double> const& vtx)         
    { 
      if(vtx.size() != 4)

	throw MCBaseException("MCShower::MotherPosition() takes std::vector<double> of length 4 only!");

      fMotherVtx = vtx;        
    }

    void MotherMomentum(std::vector<double> const& mom)         
    { 
      if(mom.size() != 4)

	throw MCBaseException("MCShower::MotherMomentum() takes std::vector<double> of length 4 only!");

      fMotherMomentum = mom;   

    }

    void DaughterTrackID(std::vector<unsigned int> const& id_v) { fDaughterTrackID = id_v; }

    void DaughterPosition(std::vector<double> const& vtx)       
    { 
      if(vtx.size() != 4)

	throw MCBaseException("MCShower::DaughterPosition() takes std::vector<double> of length 4 only!");

      fDaughterVtx = vtx;      

    }

    void DaughterMomentum(std::vector<double> const& mom)       
    { 
      if(mom.size() != 4)

	throw MCBaseException("MCShower::MotherPosition() takes std::vector<double> of length 4 only!");

      fDaughterMomentum = mom; 
    }

    void Charge(std::vector<double> const& q_v)                 { fPlaneCharge = q_v;      }

#endif


  protected:

    //---- Mother info ----//
    int fMotherPDGID;                ///< mother PDG code
    unsigned int fMotherTrackID;     ///< mother G4 Track ID
    std::string fMotherProcess;      ///< mother's creation process
    std::vector<double> fMotherVtx;  ///< mother position 4-vector @ generation
    std::vector<double> fMotherMomentum;  ///< mother momentum 4-vector @ generation

    //---- Daughter info ----//
    std::vector<unsigned int> fDaughterTrackID; ///< Daughters' track ID
    std::vector<double> fDaughterVtx;           ///< Daughters' first energy deposition vtx
    std::vector<double> fDaughterMomentum;      ///< Daughters' deposit sum momentum 4-vector

    //---- Charge per plane ----//
    std::vector<double> fPlaneCharge;           ///< Daughter's plane charge

  };

}

#endif
