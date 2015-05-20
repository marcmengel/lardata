////////////////////////////////////////////////////////////////////////////
// \version $Id: Track.cxx,v 1.5 2010/02/15 20:32:46 brebel Exp $
//
// \brief Definition of track object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Geometry/Geometry.h"
#include "Geometry/PlaneGeo.h"
#include "Geometry/WireGeo.h"
#include "RecoBase/Track.h"

#include <iomanip>
#include <iostream>

#include "TMath.h"

namespace recob{

  //----------------------------------------------------------------------
  Track::Track()
  {
  }

  //----------------------------------------------------------------------
  Track::Track(std::vector<TVector3>              const& xyz,
	       std::vector<TVector3>              const& dxdydz,
	       std::vector<std::vector <double> >        dQdx,
	       std::vector<double>                       fitMomentum,
               int                                       ID)
      : fXYZ (xyz)
      , fDir (dxdydz)
      , fdQdx(dQdx)
      , fFitMomentum(fitMomentum)
      , fID  (ID)
  {
    fCov.resize(0);

    if(fXYZ.size() != fDir.size() || fXYZ.size() < 1)
      throw cet::exception("Track Constructor") << "Position, direction vector "
						<< " size problem:\n"
						<< "\t position size = "    << fXYZ.size()
						<< "\n\t direction size = " << fDir.size() << "\n";
  }

  //----------------------------------------------------------------------
  Track::Track(std::vector<TVector3>               const& xyz,
	       std::vector<TVector3>               const& dxdydz,
	       std::vector<TMatrixT<double> >      const& cov,
	       std::vector< std::vector <double> >        dQdx,
	       std::vector<double>                        fitMomentum,
	       int                                        ID)
    : fXYZ  (xyz)
    , fDir  (dxdydz)
    , fCov  (cov)
    , fdQdx (dQdx)
    , fFitMomentum(fitMomentum)
    , fID   (ID)
  {
    if(fXYZ.size() != fDir.size() || fXYZ.size() < 1)
      throw cet::exception("Track Constructor") << "Position, direction vectors "
						<< " size problem:\n"
						<< "\t position size = "    << fXYZ.size()
						<< "\n\t direction size = " << fDir.size() << "\n";

  }

  //----------------------------------------------------------------------
  size_t Track::NumberdQdx(geo::View_t view) const
  {
    if (fdQdx.size() == 0) return 0;
    if(view == geo::kUnknown){
      mf::LogWarning("Track") << "asking for unknown view to get number of dQdX entries"
			      << " return the size for the 0th view vector";
      return fdQdx.at(0).size();
    }

    return fdQdx.at(view).size();
  }

  //----------------------------------------------------------------------
  const double& Track::DQdxAtPoint(unsigned int p,
				   geo::View_t view) const
  {
    if(view == geo::kUnknown){
      mf::LogWarning("Track") << "asking for unknown view to get number of dQdX entries"
			      << " return the size for the 0th view vector";
      return fdQdx.at(0).at(p);
    }

    return fdQdx.at(view).at(p);
  }

  //----------------------------------------------------------------------
  void Track::Extent(std::vector<double> &xyzStart,
		     std::vector<double> &xyzEnd) const
  {
    xyzStart.resize(3);
    xyzEnd.resize(3);

    xyzStart[0] = fXYZ.front().X();
    xyzStart[1] = fXYZ.front().Y();
    xyzStart[2] = fXYZ.front().Z();

    xyzEnd[0]   = fXYZ.back().X();
    xyzEnd[1]   = fXYZ.back().Y();
    xyzEnd[2]   = fXYZ.back().Z();

    return;
  }

  //----------------------------------------------------------------------
  void Track::Direction(double *dcosStart,
			double *dcosEnd) const
  {
    dcosStart[0] = fDir.front().X();
    dcosStart[1] = fDir.front().Y();
    dcosStart[2] = fDir.front().Z();

    dcosEnd[0]   = fDir.back().X();
    dcosEnd[1]   = fDir.back().Y();
    dcosEnd[2]   = fDir.back().Z();

    return;
  }

  //----------------------------------------------------------------------
  double Track::ProjectedLength(geo::View_t view) const
  {
    ///\todo CAREFUL: using view to determine projected length does not work for LBNE
    ///\todo need to think more about this
    if(view == geo::kUnknown)
      throw cet::exception("Track") << "cannot provide projected length for "
				    << "unknown view\n";

    double length = 0.;

    art::ServiceHandle<geo::Geometry> geo;
    double angleToVert = 0.;
    for(unsigned int i = 0; i < geo->Nplanes(); ++i){
      if(geo->Plane(i).View() == view){
	angleToVert = geo->Plane(i).Wire(0).ThetaZ(false) - 0.5*TMath::Pi();
	break;
      }
    }

    // now loop over all points in the trajectory and add the contribution to the
    // to the desired view
    for(size_t p = 1; p < fXYZ.size(); ++p){
      double dist = std::sqrt( pow(fXYZ[p].x() - fXYZ[p-1].x(), 2) +
			  pow(fXYZ[p].y() - fXYZ[p-1].y(), 2) +
			  pow(fXYZ[p].z() - fXYZ[p-1].z(), 2) );
      
      // (sin(angleToVert),cos(angleToVert)) is the direction perpendicular to wire
      // fDir[i-1] is the direction between the two relevant points
      double cosgamma = TMath::Abs(TMath::Sin(angleToVert)*fDir[p-1].Y() +
				   TMath::Cos(angleToVert)*fDir[p-1].Z() );
      
      /// \todo is this right, or should it be dist*cosgamma???
      length += dist/cosgamma;
    } // end loop over distances between trajectory points

    return length;
  }

  //----------------------------------------------------------------------
  // provide projected wire pitch for the view
  // by default, gives pitch at the beginning of the trajectory
  double Track::PitchInView(geo::View_t view,
			    size_t trajectory_point) const
  { 
    if(view == geo::kUnknown)
      cet::exception("Track") << "Warning cannot obtain pitch for unknown view\n";
    
    if(trajectory_point > fDir.size())
      cet::exception("Track") << "ERROR: Asking for trajectory point " 
			      << trajectory_point
			      << " when direction vector size is " 
			      << fDir.size() << ".\n";
    if(trajectory_point > fXYZ.size())
      cet::exception("Track") << "ERROR: Asking for trajectory point " 
			      << trajectory_point
			      << " when XYZ vector size is " 
			      << fXYZ.size() << ".\n";
    
    art::ServiceHandle<geo::Geometry> geo;
    int TPC  = 0;
    int Cryo = 0;
    double Position[3];
    Position[0] = fXYZ[trajectory_point].X();
    Position[1] = fXYZ[trajectory_point].Y();
    Position[2] = fXYZ[trajectory_point].Z();
    geo::TPCID tpcid = geo->FindTPCAtPosition ( Position );
    if (tpcid.isValid) {
      TPC  = tpcid.TPC;
      Cryo = tpcid.Cryostat;
    }
    double wirePitch   = geo->WirePitch(view, TPC, Cryo);
    double angleToVert = geo->WireAngleToVertical(view, TPC, Cryo) - 0.5*TMath::Pi();

    //(sin(angleToVert),cos(angleToVert)) is the direction perpendicular to wire
    double cosgamma = std::abs(std::sin(angleToVert)*fDir[trajectory_point].Y() +
			       std::cos(angleToVert)*fDir[trajectory_point].Z());

    if(cosgamma < 1.e-5)
      throw cet::exception("Track") << "cosgamma is basically 0, that can't be right\n";

    return wirePitch/cosgamma;
  }

  //----------------------------------------------------------------------
  // This is a simple summation of the distance between consecutive
  // points on the track starting with the p-th point
  // It assumes that there are sufficient points to make a
  // reasonable measurement of the length
  // This method can be combined with a particle id hypothesis to
  // get an estimate of momentum from range.
  double Track::Length(size_t p) const
  {
    double length = 0.;

    for(size_t i = p+1; i < fXYZ.size(); ++i)
      length += std::sqrt( pow(fXYZ[i].x() - fXYZ[i-1].x(), 2) +
			   pow(fXYZ[i].y() - fXYZ[i-1].y(), 2) +
			   pow(fXYZ[i].z() - fXYZ[i-1].z(), 2) );
    return length;
  }

  //----------------------------------------------------------------------
  // distance from point p on the trajectory to the end of the track



  //----------------------------------------------------------------------
  void Track::TrajectoryAtPoint(unsigned int p,
				TVector3    &pos,
				TVector3    &dir) const
  {
    pos = fXYZ.at(p);
    dir = fDir.at(p);

    return;
  }

  //----------------------------------------------------------------------
  // ostream operator.
  //
  std::ostream& operator<< (std::ostream& stream, Track const& a)
  {
    //double dcoss[3];
    //double dcose[3];
    //a.Direction(dcoss,dcose);
    TVector3 const& start = a.VertexDirection();
    TVector3 const& end   = a.EndDirection();
    stream << std::setiosflags(std::ios::fixed) << std::setprecision(3)
           << "\n Track ID "       << std::setw(4) << std::right << a.ID()
           << " Theta = "            << std::setw(6) << std::right << a.Theta()
           << " Phi = "              << std::setw(6) << std::right << a.Phi()
           << "\n  StartCosines : ( " << start.X() << " ; " << start.Y() << " ; " << start.Z()
           << ")  EndCosines : ( "   << end.X() << " ; " << end.Y() << " ; " << end.Z()
           << ")"
           << "\n  #Position and Direction = "
           << std::setw(5) << std::right << a.NumberTrajectoryPoints()
           << " #Covariance = "      << std::setw(6) << std::right << a.NumberCovariance()
           << " #dQdx = "            << std::setw(6) << std::right;
    for(size_t i = 0; i < a.fdQdx.size(); ++i)
      stream << " " << a.fdQdx.at(i).size();

    stream << std::endl;

    return stream;
  }

  //----------------------------------------------------------------------
  // Fill the global-to-local rotation matrix based on direction at point fDir[p].
  // The third axis of the local coordinate system points along the track direction.
  void Track::GlobalToLocalRotationAtPoint(unsigned int p, TMatrixD& rot) const
  {
    // Make sure matrix has the correct size.

    if(rot.GetNrows() != 3 || rot.GetNcols() != 3)
      rot.ResizeTo(3,3);

    // Calculate the global-to-local rotation matrix.

    const TVector3& dir = fDir.at(p);
    double dirmag = dir.Mag();
    double diryz = std::sqrt(dir.Y()*dir.Y() + dir.Z()*dir.Z());

    double sinth = dir.X() / dirmag;
    double costh = diryz / dirmag;
    double sinphi = 0.;
    double cosphi = 1.;
    if(diryz != 0) {
      sinphi = -dir.Y() / diryz;
      cosphi = dir.Z() / diryz;
    }
    rot(0,0) = costh;
    rot(1,0) = 0.;
    rot(2,0) = sinth;
    rot(0,1) = sinth * sinphi;
    rot(1,1) = cosphi;
    rot(2,1) = -costh * sinphi;
    rot(0,2) = -sinth * cosphi;
    rot(1,2) = sinphi;
    rot(2,2) = costh * cosphi;
  }

  //----------------------------------------------------------------------
  // Fill the local-to-global rotation matrix based on direction at point fDir[p].
  // The third axis of the local coordinate system points along the track direction.
  void Track::LocalToGlobalRotationAtPoint(unsigned int p, TMatrixD& rot) const
  {
    // Make sure matrix has the correct size.

    if(rot.GetNrows() != 3 || rot.GetNcols() != 3)
      rot.ResizeTo(3,3);

    // Calculate the global-to-local rotation matrix.

    const TVector3& dir = fDir.at(p);
    double dirmag = dir.Mag();
    double diryz = std::sqrt(dir.Y()*dir.Y() + dir.Z()*dir.Z());

    double sinth = dir.X() / dirmag;
    double costh = diryz / dirmag;
    double sinphi = 0.;
    double cosphi = 1.;
    if(diryz != 0) {
      sinphi = -dir.Y() / diryz;
      cosphi = dir.Z() / diryz;
    }
    rot(0,0) = costh;
    rot(0,1) = 0.;
    rot(0,2) = sinth;
    rot(1,0) = sinth * sinphi;
    rot(1,1) = cosphi;
    rot(1,2) = -costh * sinphi;
    rot(2,0) = -sinth * cosphi;
    rot(2,1) = sinphi;
    rot(2,2) = costh * cosphi;
  }

  //----------------------------------------------------------------------------
  bool operator < (const Track & a, const Track & b)
  {
    if(a.ID() != b. ID())
      return a.ID() < b.ID();

    return false; //They are equal
  }

}
