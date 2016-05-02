////////////////////////////////////////////////////////////////////////////
// \version $Id: Shower.h,v 1.2 2010/02/15 20:32:46 brebel Exp $
//
// \brief Definition of shower object for LArSoft
//
// \author brebel@fnal.gov, modifications by andrzej.szelc@yale.edu
//
////////////////////////////////////////////////////////////////////////////
#ifndef SHOWER_H
#define SHOWER_H

#ifndef __GCCXML__
#include <iosfwd>
#include "larcore/SimpleTypesAndConstants/PhysicalConstants.h"
#endif

#include "TVector3.h"

namespace recob {

  class Shower {

  public:

    Shower();  ///Default constructor

   private:

    int fID;
    TVector3 fDCosStart;             ///< direction cosines at start of shower
    TVector3 fSigmaDCosStart;        ///< uncertainty on initial direction cosines
    TVector3 fXYZstart;             ///< direction cosines at start of shower
    TVector3 fSigmaXYZstart;        ///< uncertainty on initial direction cosines
    std::vector< double > fTotalEnergy;           ///< Calculated Energy per each plane
    std::vector< double > fSigmaTotalEnergy;           ///< Calculated Energy per each plane
    std::vector< double > fdEdx;           ///< Calculated dE/dx per each plane
    std::vector< double > fSigmadEdx;           ///< Calculated dE/dx per each plane
    
    std::vector< double > fTotalMIPEnergy;           ///< Calculated Energy per each plane
    std::vector< double > fSigmaTotalMIPEnergy;           ///< Calculated Energy per each plane
    int fBestPlane;
    double fLength;
#ifndef __GCCXML__

  public:

    Shower(TVector3 const& dcosVtx,
	   TVector3 const& dcosVtxErr,
	   TVector3 const& xyz,
	   TVector3 const& xyzErr,
	   std::vector< double >  TotalEnergy,
	   std::vector< double >  TotalEnergyErr,
	   std::vector< double >  dEdx,
	   std::vector< double >  dEdxErr,
	   int bestplane,
	   int     id=util::kBogusI);

    
    
    //set methods
    void set_id                (const int id)                      { fID = id;                 }
    void set_total_energy      (const std::vector< double >& q)    { fTotalEnergy = q;         }
    void set_total_energy_err  (const std::vector< double >& q)    { fSigmaTotalEnergy = q;    }
    void set_total_MIPenergy   (const std::vector< double >& q)    { fTotalMIPEnergy = q;      }
    void set_total_MIPenergy_err  (const std::vector< double >& q) { fSigmaTotalMIPEnergy = q; }
    void set_total_best_plane  (const int q)                         { fBestPlane = q;        }
    
    void set_direction       (const TVector3& dir)        { fDCosStart = dir;        }
    void set_direction_err   (const TVector3& dir_e)      { fSigmaDCosStart = dir_e; }
    void set_start_point     (const TVector3& xyz)        { fXYZstart = xyz;        }
    void set_start_point_err (const TVector3& xyz_e)      { fSigmaXYZstart = xyz_e; }
    void set_dedx      (const std::vector< double >& q) { fdEdx = q;        }
    void set_dedx_err  (const std::vector< double >& q) { fSigmadEdx = q;        }
    void set_length(const double& l) { fLength = l; }    
    
    
    
    int    ID()               const;

    const TVector3& Direction()    const;
    const TVector3& DirectionErr() const;
    
    const TVector3& ShowerStart()    const;
    const TVector3& ShowerStartErr() const;

    const std::vector< double >& Energy()    const;
    const std::vector< double >& EnergyErr() const;
    
    const std::vector< double >& MIPEnergy()    const;
    const std::vector< double >& MIPEnergyErr() const;
    int    best_plane()               const;
    inline double Length() const;
    const std::vector< double >& dEdx()    const; 
    const std::vector< double >& dEdxErr() const;
    
    
    
    friend std::ostream& operator << (std::ostream& stream, Shower const& a);

    friend bool          operator <   (const Shower & a, const Shower & b);

#endif

      };
}

#ifndef __GCCXML__
   inline int    recob::Shower::ID()               const { return fID;               }

   inline const TVector3& recob::Shower::Direction()    const { return fDCosStart;          }
   inline const TVector3& recob::Shower::DirectionErr() const { return fSigmaDCosStart;     }
    
   inline const TVector3& recob::Shower::ShowerStart()    const { return fXYZstart;          }
   inline const TVector3& recob::Shower::ShowerStartErr() const { return fSigmaXYZstart;     }

   inline const std::vector< double >& recob::Shower::Energy()    const { return fTotalEnergy;          }
   inline const std::vector< double >& recob::Shower::EnergyErr() const { return fSigmaTotalEnergy;     }
    
   inline const std::vector< double >& recob::Shower::MIPEnergy()    const { return fTotalMIPEnergy;          }
   inline const std::vector< double >& recob::Shower::MIPEnergyErr() const { return fSigmaTotalMIPEnergy;     }
   inline int    recob::Shower::best_plane()               const { return fBestPlane;               }
   inline double recob::Shower::Length() const { return fLength; }    
   inline const std::vector< double >& recob::Shower::dEdx()    const { return fdEdx;          }
   inline const std::vector< double >& recob::Shower::dEdxErr() const { return fSigmadEdx;     }

#endif



#endif // SHOWER_H
