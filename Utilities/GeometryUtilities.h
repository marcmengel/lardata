////////////////////////////////////////////////////////////////////////
// \file GeometryUtilities.h
//
// \brief Functions to calculate distances and angles in 3D and 2D
//
// \author andrzej.szelc@yale.edu
//
////////////////////////////////////////////////////////////////////////
#ifndef UTIL_GEOMETRYUTILITIES_H
#define UTIL_GEOMETRYUTILITIES_H

#include "fhiclcpp/ParameterSet.h"

#include "Utilities/LArProperties.h"
#include "Geometry/Geometry.h"
#include "Utilities/DetectorProperties.h"
#include <vector>

namespace recob { 
  class Hit; 
}


///General LArSoft Utilities
namespace util{
  
    class pxpoint;
    class pxline;
  
    class GeometryUtilities {
    public:


    GeometryUtilities();
    
    ~GeometryUtilities();
      
    
    int Get3DaxisN(int iplane0,
		   int iplane1,
		   double omega0, 
		   double omega1,
		   double &phi,
		   double &theta) const;
    
    double CalculatePitch(unsigned int iplane0,
			  double phi,
			  double theta) const;
    double CalculatePitchPolar(unsigned int iplane0,
			       double phi,
			       double theta) const;
    
    double Get3DSpecialCaseTheta(int iplane0,
				 int iplane1,
				 double dw0, 
				 double dw1) const;
        
    
    double Get2Dangle(double wire,
		      double time) const;
    double Get2Dangle(double wireend,
		      double wirestart,
		      double timeend,
		      double timestart) const;
    
    double Get2Dslope(double wire,
		      double time) const;
    double Get2Dslope(double wireend,
		      double wirestart,
		      double timeend,
		      double timestart) const;
    
    double Get2DDistance(double wire1,
			 double time1,
			 double wire2,
			 double time2) const;
  
    double Get2DPitchDistance(double angle,
			      double inwire,
			      double wire) const;
    
    double Get2DPitchDistanceWSlope(double slope,
				    double inwire,
				    double wire) const;
    
    int GetPointOnLine(double slope,
		       double intercept,
		       double wire1,
		       double time1,
		       double &wireout,
		       double &timeout) const;
    
    int GetPointOnLine(double slope,
		       double wirestart,
		       double timestart,
		       double wire1,
		       double time1,
		       double &wireout,
		       double &timeout) const;
    
    int GetPointOnLineWSlopes(double slope,
			      double intercept,
			      double ort_intercept,
			      double &wireout,
			      double &timeout) const;
    
    recob::Hit* FindClosestHit(std::vector<art::Ptr< recob::Hit > > hitlist,
			       unsigned int wire,
			       double time) const;
    
    art::Ptr< recob::Hit > FindClosestHitPtr(std::vector<art::Ptr< recob::Hit > > hitlist,
				 unsigned int wirein,
				 double timein) const;			       
			       
			       
    pxpoint Get2DPointProjection(double *xyz,int plane) const;			       
	
    double GetTimeTicks(double x, int plane) const;
    
			       
    int GetPlaneAndTPC(recob::Hit*  a,
		       unsigned int &p,
		       unsigned int &cs,
		       unsigned int &t,
		       unsigned int &w) const;

     int GetPlaneAndTPC(art::Ptr<recob::Hit> a,
			unsigned int &p,
			unsigned int &cs,
			unsigned int &t,
			unsigned int &w)  const;
		       
		       
    int GetProjectedPoint(pxpoint p0,
			  pxpoint p1,
			  pxpoint &pN) const;

    int GetYZ(pxpoint p0,
	      pxpoint p1, 
	      double* yz) const;
    
    double PitchInView(unsigned int plane,
		       double phi,
		       double theta) const;
    
    void GetDirectionCosines(double phi,
			     double theta,
			     double *dirs) const;
			     
    void SelectLocalHitlist(std::vector< art::Ptr < recob::Hit> > hitlist, 
			    std::vector < art::Ptr<recob::Hit> > &hitlistlocal,
			    double wire_start,
			    double time_start, 
			    double linearlimit,   
			    double ortlimit, 
			    double lineslopetest);			     
	
   double TimeToCm() {return fTimetoCm;};
   double WireToCm() {return fWiretoCm;};			     
    
  private:

    art::ServiceHandle<geo::Geometry> geom; 
    art::ServiceHandle<util::DetectorProperties> detp; 
    art::ServiceHandle<util::LArProperties> larp; 

    std::vector< double > vertangle;  //angle wrt to vertical
    double fWirePitch;
    double fTimeTick;
    double fDriftVelocity;
    unsigned int fNPlanes;
    double fWiretoCm;
    double fTimetoCm;
    double fWireTimetoCmCm;
    
    }; // class GeometryUtilities




    //helper class needed for the endpoint finding
    class pxpoint {
    public:
      double w;
      double t;
      unsigned int plane;
   
      pxpoint(){
	plane=0;
	w=0;
	t=0;
      }
      
      pxpoint(int pp,double ww,double tt){
	plane=pp;
	w=ww;
	t=tt;
      }
    
    };
    
    
    //helper class needed for the seeding
    class pxline {
    public:
      
      pxpoint pt0() { return pxpoint(plane,w0,t0); }
      pxpoint pt1() { return pxpoint(plane,w1,t1); }
      
      double w0; ///<defined to be the vertex w-position
      double t0; ///<defined to be the vertex t-position
      double w1; ///<defined to be the ending w-position (of line or seed depending)
      double t1; ///<defined to be the ending t-position (of line or seed depending)
      unsigned int plane;
   
      pxline(int pp,double ww0,double tt0, double ww1, double tt1){
	plane=pp;
	w0=ww0;
	t0=tt0;
	w1=ww1;
	t1=tt1;
      }
    
      pxline(){
	plane=0;
	w0=0;
	t0=0;
	w1=0;
	t1=0;
      }
    
    friend ostream &operator<<(ostream &out, pxline pline)     //output
     {
        out<<"pl:"<<pline.plane <<  " ("<<pline.w0<<","<<pline.t0<<")->("<<pline.w1<<","<<pline.t1<<")";
        return out;
     }
    
    };
    

     
     
     
} //namespace utils
#endif // UTIL_DETECTOR_PROPERTIES_H
