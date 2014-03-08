////////////////////////////////////////////////////////////////////////////
// \version $Id: Cluster.cxx,v 1.7 2010/06/12 21:46:34 spitz7 Exp $
//
// \brief Definition of cluster object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "RecoBase/Cluster.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "TMath.h"

namespace recob{

  //----------------------------------------------------------------------
  Cluster::Cluster()
    : fTotalCharge(-999.)
    , fdTdW(0.)
    , fdQdW(0.)
    , fID(-1)
    , fView(geo::kUnknown)
  {

  }

  //----------------------------------------------------------------------
  Cluster::Cluster(double startWire, double sigmaStartWire,
       double startTime, double sigmaStartTime,
       double endWire, double sigmaEndWire,
       double endTime, double sigmaEndTime,
       double dTdW, double sigmadTdW,
       double dQdW, double sigmadQdW,
       double totalQ,
       geo::View_t view,
       int id)
    : fTotalCharge(totalQ)
    , fdTdW(dTdW)
    , fdQdW(dQdW)
    , fSigmadTdW(sigmadTdW)
    , fSigmadQdW(sigmadQdW)
    , fID(id)
    , fView(view)
  {

    fStartPos.push_back(startWire);
    fStartPos.push_back(startTime);
    fSigmaStartPos.push_back(sigmaStartWire);
    fSigmaStartPos.push_back(sigmaStartTime);

    fEndPos.push_back(endWire);
    fEndPos.push_back(endTime);
    fSigmaEndPos.push_back(sigmaEndWire);
    fSigmaEndPos.push_back(sigmaEndTime);
  }

  //----------------------------------------------------------------------
  //  Addition operator.
  //
  Cluster Cluster::operator +(Cluster a)
  {

    // throw exception if the clusters are not from the same view
    if( a.View() != this->View() )
      throw cet::exception("Cluster+operator") << "Attempting to sum clusters from "
                 << "different views is not allowed";

    // check the start and end positions - for now the
    // smallest wire number means start position, largest means end position
    std::vector<double> astart(a.StartPos());
    std::vector<double> aend  (a.EndPos()  );
    std::vector<double> start(StartPos());
    std::vector<double> end  (EndPos()  );
    std::vector<double> sigstart(SigmaStartPos());
    std::vector<double> sigend  (SigmaEndPos()  );

    if(astart[0] < fStartPos[0]){
      start = astart;
      sigstart = a.SigmaStartPos();
    }

    if(aend[0] > fEndPos[0]){
      end = aend;
      sigend = a.SigmaEndPos();
    }

    //take weighted mean in obtaining average slope and differential charge,
    //based on total charge each cluster
    double dtdw = ((this->Charge()*dTdW()) + (a.Charge()*a.dTdW()))/(this->Charge() + a.Charge());
    double dqdw = ((this->Charge()*dQdW()) + (a.Charge()*a.dQdW()))/(this->Charge() + a.Charge());

    //hits.sort();//sort the PtrVector to organize Hits of new Cluster
    double sigdtdw = TMath::Max(SigmadTdW(), a.SigmadTdW());
    double sigdqdw = TMath::Max(SigmadQdW(), a.SigmadQdW());

    Cluster sum(//hits,
    start[0], sigstart[0],
    start[1], sigstart[1],
    end[0],   sigend[0],
    end[1],   sigend[1],
    dtdw, sigdtdw,
    dqdw, sigdqdw,
    this->Charge() + a.Charge(),
    this->View(),
    ID());

    return sum;

  }

  //----------------------------------------------------------------------
  // ostream operator.
  //
  std::ostream& operator<< (std::ostream& o, const Cluster& c)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << "Cluster ID "    << std::setw(5)  << std::right << c.ID()
      << " : View = "     << std::setw(3)  << std::right << c.View()
      << " StartWire = "  << std::setw(7)  << std::right << c.StartPos()[0]
      << " EndWire = "    << std::setw(7)  << std::right << c.EndPos()[0]
      << " StartTime = "  << std::setw(9)  << std::right << c.StartPos()[1]
      << " EndTime = "    << std::setw(9)  << std::right << c.EndPos()[1]
      << " dTdW = "       << std::setw(9)  << std::right << c.dTdW()
      << " dQdW = "       << std::setw(9)  << std::right << c.dQdW()
      << " Charge = "     << std::setw(10) << std::right << c.Charge();

    return o;
  }


  //----------------------------------------------------------------------
  // < operator.
  //
  bool operator < (const Cluster & a, const Cluster & b)
  {
    if(a.View() != b.View())
      return a.View() < b.View();
    if(a.ID() != b. ID())
      return a.ID() < b.ID();
    if(a.StartPos()[0] != b.StartPos()[0])
      return a.StartPos()[0] < b.StartPos()[0];
    if(a.EndPos()[0] != b.EndPos()[0])
      return a.EndPos()[0] < b.EndPos()[0];

    return false; //They are equal
  }

}// namespace

