////////////////////////////////////////////////////////////////////////
// \version $Id$
//
// \brief Definition of Calorimetry analysis object
//
// \author brebel@fnal.gov, tjyang@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "AnalysisBase/Calorimetry.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace anab{

  //----------------------------------------------------------------------
  Calorimetry::Calorimetry() 
    : fKineticEnergy(0.)
    , fRange(0.)
  {
    mf::LogWarning("AnaBaseDefaultCtor") << "using default Calorimetry ctor - should only ever"
					  << " be done when getting hits out of an event"
					  << " not when trying to produce new hits to store"
					  << " in the event";
    fdEdx.clear();
    fdQdx.clear();
    fResidualRange.clear();
    fDeadWireResR.clear();
    fTrkPitch.clear();

  }


 //----------------------------------------------------------------------
  Calorimetry::Calorimetry(double KineticEnergy,
			   std::vector<double> const& dEdx,
			   std::vector<double> const& dQdx,
			   std::vector<double> const& resRange,
			   std::vector<double> const& deadwire,
			   double Range,
			   double TrkPitch) 
  {
 
    fKineticEnergy = KineticEnergy;
    fRange = Range;
    for(size_t i=0; i!=dQdx.size(); ++i) fTrkPitch.push_back(TrkPitch);
    if(dEdx.size() != resRange.size())
      throw cet::exception("anab::Calorimetry") << "dE/dx and residual range vectors "
						<< "have different sizes, this is a problem.";
    fdEdx.resize(dEdx.size());
    fdQdx.resize(dQdx.size());
    fResidualRange.resize(resRange.size());
    for(size_t i = 0; i < dEdx.size(); ++i){
      fdEdx[i]          = dEdx[i];
      fdQdx[i]          = dQdx[i];
      fResidualRange[i] = resRange[i];
    }
    
    fDeadWireResR.resize(deadwire.size());
    for(size_t i = 0; i<deadwire.size(); ++i){
      fDeadWireResR[i] = deadwire[i];
    }

  }


  //----------------------------------------------------------------------
  Calorimetry::Calorimetry(double KineticEnergy,
			   std::vector<double> const& dEdx,
			   std::vector<double> const& dQdx,
			   std::vector<double> const& resRange,
			   std::vector<double> const& deadwire,
			   double Range,
			   std::vector<double> const& TrkPitch) 
  {
    
    fKineticEnergy = KineticEnergy;
    fRange = Range;
    fTrkPitch = TrkPitch;
    if(dEdx.size() != resRange.size())
      throw cet::exception("anab::Calorimetry") << "dE/dx and residual range vectors "
						<< "have different sizes, this is a problem.";
    fdEdx.resize(dEdx.size());
    fdQdx.resize(dQdx.size());
    fResidualRange.resize(resRange.size());

    for(size_t i = 0; i < dEdx.size(); ++i){
      fdEdx[i]          = dEdx[i];
      fdQdx[i]          = dQdx[i];
      fResidualRange[i] = resRange[i];
    }
    
    fDeadWireResR.resize(deadwire.size());
    for(size_t i = 0; i<deadwire.size(); ++i){
      fDeadWireResR[i] = deadwire[i];
    }

  }

  //----------------------------------------------------------------------
  // ostream operator.  
  //
  std::ostream& operator<< (std::ostream & o, Calorimetry const& a)
  {
    o << "Kinetic Energy: " << a.fKineticEnergy 
      << "\n Range: "         << a.fRange << std::endl;
    
    for(size_t n = 0; n < a.fdEdx.size(); ++n)
      o << "dE/dx: "           << a.fdEdx[n]
	<< " Residual range: " << a.fResidualRange[n] << std::endl;

    return o;
  }
  
}
