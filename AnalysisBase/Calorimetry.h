////////////////////////////////////////////////////////////////////////////
// \version 
//
// \brief Definition of data product to hold Calorimetry information
//
// \author brebel@fnal.gov, tjyang@fnal.gov
//
////////////////////////////////////////////////////////////////////////////
#ifndef ANAB_CALORIMETRY_H
#define ANAB_CALORIMETRY_H

#include <vector>
#include <iosfwd>
#include <iostream>
#include <iomanip>

namespace anab {

  class Calorimetry{
  public:
    
    Calorimetry();

    double              fKineticEnergy;   ///< determined kinetic energy
    std::vector<double> fdEdx;            ///< dE/dx, should be same size as fResidualRange
    std::vector<double> fdQdx;            ///< dQ/dx
    std::vector<double> fResidualRange;   ///< range from end of track    
    std::vector<double> fDeadWireResR;    ///< dead wire residual range, collection plane
    double              fRange;           ///< total range of track
    std::vector<double> fTrkPitch;        ///< track pitch on collection plane

#ifndef __GCCXML__
  public:

    Calorimetry(double KinematicEnergy,
		std::vector<double> const& dEdx,
		std::vector<double> const& dQdx,
		std::vector<double> const& resRange,
		std::vector<double> const& deadwire,
		double Range,
		double TrkPitch);

    Calorimetry(double KineticEnergy,
		std::vector<double> const& dEdx,
		std::vector<double> const& dQdx,
		std::vector<double> const& resRange,
		std::vector<double> const& deadwire,
		double Range,
		std::vector<double> const& TrkPitch);

    friend std::ostream& operator << (std::ostream &o, Calorimetry const& a);

    const std::vector<double>& dEdx()          const; 
    const std::vector<double>& dQdx()          const; 
    const std::vector<double>& ResidualRange() const; 
    const std::vector<double>& DeadWireResRC() const; 
    const double&              KineticEnergy() const; 
    const double&              Range()         const; 
    double                     TrkPitchC()     const; 
    const std::vector<double>& TrkPitchVec()   const;

#endif
    
  };

}

#ifndef __GCCXML__

inline const std::vector<double>& anab::Calorimetry::dEdx()          const { return fdEdx;          }
inline const std::vector<double>& anab::Calorimetry::dQdx()          const { return fdQdx;          }
inline const std::vector<double>& anab::Calorimetry::ResidualRange() const { return fResidualRange; }
inline const std::vector<double>& anab::Calorimetry::DeadWireResRC() const { return fDeadWireResR;  }
inline const double&              anab::Calorimetry::KineticEnergy() const { return fKineticEnergy; } 
inline const double&              anab::Calorimetry::Range()         const { return fRange;         }
inline const std::vector<double>& anab::Calorimetry::TrkPitchVec()   const { return fTrkPitch;      }
inline double                     anab::Calorimetry::TrkPitchC()     const 
{ 
  if (fTrkPitch.size()) 
    return fTrkPitch[0];
  else return 0;
}

#endif

#endif //ANAB_CALORIMETRY_H
