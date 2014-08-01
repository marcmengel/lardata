////////////////////////////////////////////////////////////////////////////
// \version $Id: PFParticle.h,v 1.0 2014/07/23 12:00:00 usher Exp $
//
// \brief Definition of PFParticle object for LArSoft
//
// \author usher@slac.stanford.edu
//
////////////////////////////////////////////////////////////////////////////

#ifndef Recob_PFParticle_H
#define Recob_PFParticle_H

#include <vector>
#include <ostream>
#include <climits>

namespace recob {

  class PFParticle {

  public:

      PFParticle();  /// Default constructor necessary for gccxml - not really for public use

  private:

      int                 fPdgCode;   // A preliminary estimate of the PFParticle type using the PDG code
      size_t              fSelf;      // Self reference
      size_t              fParent;    // Index into PFParticle collection for parent
      std::vector<size_t> fDaughters; // Vector of indices into PFParticle Collection for daughters

#ifndef __GCCXML__

  public:
      
    // Define index to signify primary particle
    enum {kPFParticlePrimary = UINT_MAX};
      
    /// Primary constructor
    PFParticle(int pdgCode, size_t self, size_t parent, const std::vector<size_t>& daughters);
      
    /// Destructor definition
     ~PFParticle() = default;
      
    /// Copy constructor
    PFParticle(const PFParticle& other) = default;
    PFParticle(PFParticle&& other)      = default;
      
    /// Copy assignment operator
    PFParticle& operator= (const PFParticle& other) = default;
    PFParticle& operator= (PFParticle&& other)      = default;
    
    /// Accessors
    int                        PdgCode()            const {return fPdgCode;}
    bool                       IsPrimary()          const {return fParent == PFParticle::kPFParticlePrimary;}
    int                        NumDaughters()       const {return fDaughters.size();}
    size_t                     Self()               const {return fSelf;}
    size_t                     Parent()             const {return fParent;}
    size_t                     Daughter(size_t idx) const;
    const std::vector<size_t>& Daughters()          const {return fDaughters;}

    friend std::ostream& operator << (std::ostream& o, const PFParticle& c);
    friend bool          operator <  (const PFParticle& a, const PFParticle& b); // This is needed for art?
    
#endif
  }; // class PFParticle
} // namespace recob

#endif //Recob_PFParticle_H
