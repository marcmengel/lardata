/**
 * \file MCStep.h
 *
 * \ingroup MCBase
 * 
 * \brief Class def header for mcstep data container
 *
 * @author Kazu - Nevis 2014
 */

/** \addtogroup MCStep

    @{*/

#ifndef MCSTEP_H
#define MCSTEP_H

#include <vector>
#include <TLorentzVector.h>
#include <string>
#include "MCLimits.h"

namespace sim{

  class MCStep {

  public:

    MCStep() {};
    
    MCStep(const TLorentzVector& p, const TLorentzVector& m)
      : _position(p),
	_momentum(m)
    {}
    
    virtual ~MCStep(){}

    #ifndef __GCCXML__
    const TLorentzVector& Momentum() const {return _momentum;}
    
    const TLorentzVector& Position() const {return _position;}
    
    double X()  const {return _position.X();  }
    double Y()  const {return _position.Y();  }
    double Z()  const {return _position.Z();  }
    double T()  const {return _position.T();  }
    double Px() const {return _momentum.Px(); }
    double Py() const {return _momentum.Py(); }
    double Pz() const {return _momentum.Pz(); }
    double E() const  {return _momentum.E();  }
    
    void SetPosition(const TLorentzVector& v) { _position=v; };
    void SetMomentum(const TLorentzVector& v) { _momentum=v; };

    #endif

  protected:
    
    TLorentzVector _position; // Position 4-vector (x,y,z,t) in [cm,cm,cm,ns]
    TLorentzVector _momentum; // Momentum 4-vector (px,py,pz,E) in [MeV/c,MeV/c,Mev/c,MeV]
    
  };
}

#endif

/** @} */ // end of doxygen group 
