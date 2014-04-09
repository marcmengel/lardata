////////////////////////////////////////////////////////////////////////////
// \version $Id: Cluster.cxx,v 1.7 2010/06/12 21:46:34 spitz7 Exp $
//
// \brief Definition of cluster object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#include "RecoBase/Cluster.h"

#include <iomanip>

#include "cetlib/exception.h"

#include "TMath.h"

namespace recob{

  //----------------------------------------------------------------------
  Cluster::Cluster()
    : fTotalCharge(-999.)
    , fdTdW(0.)
    , fdQdW(0.)
    , fID(-1)
    , fView(geo::kUnknown)
    , fPlaneID()
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
       int id,
       const geo::PlaneID& planeID)
    : fTotalCharge(totalQ)
    , fdTdW(dTdW)
    , fdQdW(dQdW)
    , fSigmadTdW(sigmadTdW)
    , fSigmadQdW(sigmadQdW)
    , fID(id)
    , fView(view)
    , fPlaneID(planeID)
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

  // Version with default plane ID (legacy)
  Cluster::Cluster(double startWire, double sigmaStartWire,
       double startTime, double sigmaStartTime,
       double endWire, double sigmaEndWire,
       double endTime, double sigmaEndTime,
       double dTdW, double sigmadTdW,
       double dQdW, double sigmadQdW,
       double totalQ,
       geo::View_t view,
       int id)
    : Cluster(startWire, sigmaStartWire, startTime, sigmaStartTime,
              endWire, sigmaEndWire, endTime, sigmaEndTime,
              dTdW, sigmadTdW, dQdW, sigmadQdW, totalQ, view, id, geo::PlaneID())
    {}

  //----------------------------------------------------------------------
  //  Addition operator.
  //
  // The two clusters must have the same view and must lay on the same plane.
  // If one of the clusters has an invalid plane, the sum will inherit the
  // other's plane. If both are invalid, sum will also have an invalid plane.
  //
  Cluster Cluster::operator +(const Cluster& a)
  {

    // throw exception if the clusters are not from the same view
    if( a.View() != this->View() )
      throw cet::exception("Cluster+operator") << "Attempting to sum clusters from "
                 << "different views is not allowed\n";

    if ( a.hasPlane() && hasPlane() && (a.Plane() != Plane()))
      throw cet::exception("Cluster+operator") << "Attempting to sum clusters from "
                 << "different planes is not allowed\n";
    
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

    return Cluster (//hits,
      start[0], sigstart[0],
      start[1], sigstart[1],
      end[0],   sigend[0],
      end[1],   sigend[1],
      dtdw, sigdtdw,
      dqdw, sigdqdw,
      this->Charge() + a.Charge(),
      this->View(),
      ID(),
      hasPlane()? Plane(): a.Plane()
      );

  } // Cluster::operator+ ()

  //----------------------------------------------------------------------
  // ostream operator.
  //
  std::ostream& operator<< (std::ostream& o, const Cluster& c)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << "Cluster ID "    << std::setw(5)  << std::right << c.ID()
      << " : Cryo = "     << std::setw(3)  << std::right << c.Plane().Cryostat
      << " TPC = "        << std::setw(3)  << std::right << c.Plane().TPC
      << " Plane = "      << std::setw(3)  << std::right << c.Plane().Plane
      << " View = "       << std::setw(3)  << std::right << c.View() 
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
    if (a.hasPlane() && b.hasPlane() && a.Plane() != b.Plane())
      return a.Plane() < b.Plane();
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

