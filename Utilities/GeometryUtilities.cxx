////////////////////////////////////////////////////////////////////////
//  \file GeometryUtilities.cxx 
//
//  \brief Functions to calculate distances and angles in 3D and 2D
//
// andrzej.szelc@yale.edu
//
////////////////////////////////////////////////////////////////////////
#include "messagefacility/MessageLogger/MessageLogger.h"
// 

#include "TMath.h"

// LArSoft includes
#include "RecoBase/Hit.h"
#include "Utilities/GeometryUtilities.h"
#include "Geometry/CryostatGeo.h"
#include "Geometry/PlaneGeo.h"
#include "Geometry/WireGeo.h"
#include "Geometry/TPCGeo.h"
#include "SimpleTypesAndConstants/geo_types.h"
#include "art/Persistency/Common/Ptr.h" 

#include <stdint.h>

namespace util{

  //--------------------------------------------------------------------
  GeometryUtilities::GeometryUtilities() 
  {
    art::ServiceHandle<geo::Geometry> geom; 
    art::ServiceHandle<util::DetectorProperties> detp; 
    art::ServiceHandle<util::LArProperties> larp; 
    
    fNPlanes = geom->Nplanes();
    vertangle.resize(fNPlanes);
    for(unsigned int ip=0;ip<fNPlanes;ip++)
      vertangle[ip]=geom->Plane(ip).Wire(0).ThetaZ(false)-TMath::Pi()/2.; // wire angle 
        
    fWirePitch = geom->WirePitch(0,1,0);
    fTimeTick=detp->SamplingRate()/1000.; 
    fDriftVelocity=larp->DriftVelocity(larp->Efield(),larp->Temperature());
    
    fWiretoCm=fWirePitch;
    fTimetoCm=fTimeTick*fDriftVelocity;
    fWireTimetoCmCm=(fTimeTick*fDriftVelocity)/fWirePitch;
	  
    ////std::cout << " --- getting geom info " << fWirePitch << std::endl;
  }

  //--------------------------------------------------------------------
  GeometryUtilities::~GeometryUtilities() 
  {
    
  }
  

  //-----------------------------------------------------------------------------
  // omega0 and omega1 are calculated as:
  //  angle based on distances in wires and time - rescaled to cm.
  // tan(angle)*fMean_wire_pitch/(fTimeTick*fDriftVelocity);
  // as those calculated with Get2Dangle
  // writes phi and theta in degrees.
  /////////////////////////////////////
  int GeometryUtilities::Get3DaxisN(int iplane0,
				    int iplane1,
				    double omega0, 
				    double omega1,
				    double &phi,
				    double &theta) const
  {
 
double l(0),m(0),n(0);
	double ln(0),mn(0),nn(0);
	double phis(0),thetan(0);
	//double phin(0);//,phis(0),thetan(0);
  	// pretend collection and induction planes. 
 	// "Collection" is the plane with the vertical angle equal to zero. 
 	// If both are non zero collection is the one with the negative angle. 
	unsigned int Cplane=0,Iplane=1;   
 	//then angleC and angleI are the respective angles to vertical in these planes and slopeC, slopeI are the tangents of the track.
	double angleC,angleI,slopeC,slopeI,omegaC,omegaI;
 	
	// don't know how to reconstruct these yet, so exit with error.
	
	if(omega0==0 || omega1==0){
		phi=0;
		theta=-999;
		return -1;
	}
	
 
 	//////////insert check for existence of planes.
 
 	//check if backwards going track
	double backwards=0;

	double alt_backwards=0;
	
	///// or?
	if(fabs(omega0)>(TMath::Pi()/2.0) && fabs(omega1)>(TMath::Pi()/2.0) ) {
		backwards=1;
	}
	
	if(fabs(omega0)>(TMath::Pi()/2.0) || fabs(omega1)>(TMath::Pi()/2.0) ) {
		alt_backwards=1;
	}

	
	
 
	if(vertangle[iplane0] == 0){   
   		// first plane is at 0 degrees
		Cplane=iplane0;
		Iplane=iplane1;
		omegaC=omega0;
		omegaI=omega1;
	}
	else if(vertangle[iplane1] == 0){  
   		// second plane is at 0 degrees
		Cplane = iplane1;
		Iplane = iplane0;
		omegaC=omega1;
		omegaI=omega0;
	}
	else if(vertangle[iplane0] != 0 && vertangle[iplane1] != 0){
   		//both planes are at non zero degree - find the one with deg<0
		if(vertangle[iplane1] < vertangle[iplane0]){
			Cplane = iplane1;
			Iplane = iplane0;
			omegaC=omega1;
			omegaI=omega0;
		}
		else if(vertangle[iplane1] > vertangle[iplane0]){
			Cplane = iplane0;
			Iplane = iplane1;
			omegaC=omega0;
			omegaI=omega1;
		}
		else{
     			//throw error - same plane.
			return -1;
		}	

	}
	slopeC=tan(omegaC);
	slopeI=tan(omegaI);
	//omega0=tan(omega0);
	//omega1=tan(omega1);
	angleC=vertangle[Cplane];
	angleI=vertangle[Iplane];
	
 	//0 -1 factor depending on if one of the planes is vertical.
	bool nfact = !(vertangle[Cplane]);

	
	
	if(omegaC < TMath::Pi() && omegaC > 0 )
	  ln=1;
	else
	  ln=-1;
	
	//std::cout << " slopes, C:"<< slopeC << " " << (omegaC) << " I:" << slopeI << " " << omegaI <<std::endl;
	slopeI=tan(omegaI);
	
	//std::cout << "omegaC, angleC " << omegaC << " " << angleC << "cond: " << omegaC-angleC << " ln: " << ln << std::endl;
	
	l = 1;
 
	
	m = (1/(2*sin(angleI)))*((cos(angleI)/(slopeC*cos(angleC)))-(1/slopeI) 
			+nfact*(  cos(angleI)/slopeC-1/slopeI  )     );
 	
	n = (1/(2*cos(angleC)))*((1/slopeC)+(1/slopeI) +nfact*((1/slopeC)-(1/slopeI)));
 
	mn = (ln/(2*sin(angleI)))*((cos(angleI)/(slopeC*cos(angleC)))-(1/slopeI) 
			+nfact*(  cos(angleI)/(cos(angleC)*slopeC)-1/slopeI  )     );
 	
	nn = (ln/(2*cos(angleC)))*((1/slopeC)+(1/slopeI) +nfact*((1/slopeC)-(1/slopeI)));
 
	
	float Phi;
	float alt_Phi;
 	// Direction angles
	if(fabs(angleC)>0.01)  // catch numeric error values 
	{
	phi=atan(n/l);
	//phin=atan(ln/nn);
	phis=asin(ln/TMath::Sqrt(ln*ln+nn*nn));
        
	if(fabs(slopeC+slopeI) < 0.001)
	  phis=0;
	else if(fabs(omegaI)>0.01 && (omegaI/fabs(omegaI) == -omegaC/fabs(omegaC) ) && ( fabs(omegaC) < 20*TMath::Pi()/180 || fabs(omegaC) > 160*TMath::Pi()/180   ) ) // angles have 
	  {phis = (fabs(omegaC) > TMath::Pi()/2) ? TMath::Pi() : 0;    //angles are 
	
	  }
	
	
	
	if(nn<0 && phis>0 && !(!alt_backwards && fabs(phis)<TMath::Pi()/4 ) )   // do not go back if track looks forward and phi is forward
	  phis=(TMath::Pi())-phis;
	else if(nn<0 && phis<0 && !(!alt_backwards && fabs(phis)<TMath::Pi()/4 ) )
	  phis=(-TMath::Pi())-phis;
	  
	
	// solve the ambiguities due to tangent periodicity
	Phi = phi > 0. ? (TMath::Pi()/2)-phi : fabs(phi)-(TMath::Pi()/2) ; 
	alt_Phi = phi > 0. ? (TMath::Pi()/2)-phi : fabs(phi)-(TMath::Pi()/2) ; 
	
	if(backwards==1){
		if(Phi<0){ Phi=Phi+TMath::Pi();}
		else if(Phi>0){Phi=Phi-TMath::Pi();}
	}
	
	bool alt_condition=( ( fabs(omegaC)>0.75*TMath::Pi() && fabs(omegaI)>0.166*TMath::Pi() )|| ( fabs(omegaI)>0.75*TMath::Pi() && fabs(omegaC)>0.166*TMath::Pi() ) );
	
	
	if((alt_backwards==1 && alt_condition)   || backwards==1 ){
		if(alt_Phi<0){alt_Phi=alt_Phi+TMath::Pi();}
		else if(alt_Phi>0){alt_Phi=alt_Phi-TMath::Pi();}
	}
	
	}
	else  // if plane is collection than Phi = omega
	{phi=omegaC;
	Phi=omegaC;
	phis=omegaC;
	alt_Phi=omegaC;
	}
	
	
	theta = acos( m / (sqrt(pow(l,2)+pow(m,2)+pow(n,2)) ) ) ;
	thetan = -asin ( mn / (sqrt(pow(l,2)+pow(mn,2)+pow(nn,2)) ) ) ;
	//double thetah = acos( mn / (sqrt(pow(l,2)+pow(mn,2)+pow(nn,2)) ) ) ;
	//float Theta;
	//float alt_Theta = 0.;
	
	  
	
	
	//if(Phi < 0)Theta = (TMath::Pi()/2)-theta;
	//if(Phi > 0)Theta = theta-(TMath::Pi()/2);

	//if(alt_Phi < 0)alt_Theta = (TMath::Pi()/2)-theta;
	//if(alt_Phi > 0)alt_Theta = theta-(TMath::Pi()/2);
	
	////std::cout << "++++++++ GeomUtil " << Phi*180/TMath::Pi() << " " << Theta*180/TMath::Pi() << std::endl;
	//std::cout << "++++++++ GeomUtil_angles: Phi: " << alt_Phi*180/TMath::Pi() << " Theta: " << alt_Theta*180/TMath::Pi() << std::endl;
	
	//std::cout << "++++++++ GeomUtil_new_angles: Phi: " << phis*180/TMath::Pi() << " Theta: " << thetan*180/TMath::Pi() << std::endl;
	
	phi=phis*180/TMath::Pi();
	theta=thetan*180/TMath::Pi();
 
 
	return 0;   }

  //////////////////////////////
  //Calculate theta in case phi~0
  //returns theta in angles
  ////////////////////////////////
  double GeometryUtilities::Get3DSpecialCaseTheta(int iplane0,
						  int iplane1,
						  double dw0, 
						  double dw1) const
  {
    art::ServiceHandle<geo::Geometry> geom; 

  
    double splane,lplane;   // plane in which the track is shorter and longer.
    double sdw,ldw; 
  
    if(dw0==0 && dw1==0)
      return -1;
  
    if(dw0 >= dw1 ) {
      lplane=iplane0; 
      splane=iplane1;
      ldw=dw0;
      sdw=dw1;
    }
    else {
      lplane=iplane1; 
      splane=iplane0;
      ldw=dw1;
      sdw=dw0;
    }
  
    double top=(std::cos(vertangle[splane])-std::cos(vertangle[lplane])*sdw/ldw);
    double bottom = tan(vertangle[lplane]*std::cos(vertangle[splane]) ); 
          bottom -= tan(vertangle[splane]*std::cos(vertangle[lplane]) )*sdw/ldw;
  
    double tantheta=top/bottom;
  
    return atan(tantheta)*vertangle[lplane]/std::abs(vertangle[lplane])*180./TMath::Pi();
  }

  /////////////////////////////////////////////////////////
  //Calculate 3D pitch in beam coordinates
  // 
  /////////////////////////////////////////////////////////
  double GeometryUtilities::CalculatePitch(unsigned int iplane,
					   double phi,
					   double theta) const
  {
 
    double pitch = -1.;
  
    if(geom->Plane(iplane).View() == geo::kUnknown || 
       geom->Plane(iplane).View() == geo::k3D){
      mf::LogWarning("GeometryUtilities")<< "Warning :  no Pitch foreseen for view " 
					 <<geom->Plane(iplane).View();
      return pitch;
    }
    else{
     
      double pi=TMath::Pi();
     
      double fTheta=pi/2-theta;
     
      double fPhi=-(phi+pi/2);
      //double fPhi=pi/2-phi;
      //if(fPhi<0)
      //	fPhi=phi-pi/2;
     
      //fTheta=TMath::Pi()/2;
 
     
     
      for(unsigned int cs = 0; cs < geom->Ncryostats(); ++cs){
	for(unsigned int t = 0; t < geom->Cryostat(cs).NTPC(); ++t){
	  for(unsigned int i = 0; i < geom->Cryostat(cs).TPC(t).Nplanes(); ++i){
	    if(i == iplane){
	      double wirePitch = geom->Cryostat(cs).TPC(t).WirePitch(0,1,i);
	      double angleToVert =0.5*TMath::Pi() - geom->Cryostat(cs).TPC(t).Plane(i).Wire(0).ThetaZ(false) ;

	      // 	//    //std::cout <<" %%%%%%%%%%  " << i << " angle " 
	      // 				       << angleToVert*180/pi << " " 
	      // 				       << geom->Plane(i).Wire(0).ThetaZ(false)*180/pi 
	      // 				       <<" wirePitch " << wirePitch
	      // 				       <<"\n %%%%%%%%%%  " << fTheta << " " << fPhi<< std::endl;
	      // 	   
	    
	      double cosgamma = TMath::Abs(TMath::Sin(angleToVert)*TMath::Cos(fTheta)
					   +TMath::Cos(angleToVert)*TMath::Sin(fTheta)*TMath::Sin(fPhi));
	     
	      if (cosgamma>0) pitch = wirePitch/cosgamma;     
	    } // end if the correct view
	  } // end loop over planes
	} // end loop over TPCs
      } // end loop over cryostats
    } // end if a reasonable view
   
    return pitch;
  }



  /////////////////////////////////////////////////////////
  //Calculate 3D pitch in polar coordinates
  // 
  /////////////////////////////////////////////////////////
  double GeometryUtilities::CalculatePitchPolar(unsigned int iplane,
						double phi,
						double theta) const
  {
 
    double pitch = -1.;
  
    if(geom->Plane(iplane).View() == geo::kUnknown || 
       geom->Plane(iplane).View() == geo::k3D){
      mf::LogWarning("GeometryUtilities")<< "Warning :  no Pitch foreseen for view " 
					 << geom->Plane(iplane).View();
      return pitch;
    }
    else{
        
      double fTheta=theta;
      double fPhi=phi;  
     
     
      //fTheta=TMath::Pi()/2;
     
     
     
      for(unsigned int cs = 0; cs < geom->Ncryostats(); ++cs){
	for(unsigned int t = 0; t < geom->Cryostat(cs).NTPC(); ++t){
	  for(unsigned int i = 0; i < geom->Cryostat(cs).TPC(t).Nplanes(); ++i){
	    if(i == iplane){
	      double wirePitch = geom->Cryostat(cs).TPC(t).WirePitch(0,1,i);
	      double angleToVert =0.5*TMath::Pi() - geom->Cryostat(cs).TPC(t).Plane(i).Wire(0).ThetaZ(false) ;

	      // 	    //std::cout <<" %%%%%%%%%%  " << i << " angle " 
	      // 				       << angleToVert*180/pi << " " 
	      // 				       << geom->Plane(i).Wire(0).ThetaZ(false)*180/pi 
	      // 				       <<" wirePitch " << wirePitch
	      // 				       <<"\n %%%%%%%%%%  " << fTheta << " " << fPhi<< std::endl;
	   
	    
	      double cosgamma = TMath::Abs(TMath::Sin(angleToVert)*TMath::Cos(fTheta)
					   +TMath::Cos(angleToVert)*TMath::Sin(fTheta)*TMath::Sin(fPhi));
	     
	      if (cosgamma>0) pitch = wirePitch/cosgamma;     
	    } // end if the correct view
	  } // end loop over planes
	} // end loop over TPCs
      } // end loop over cryostats
    } // end if a reasonable view
   
    return pitch;
  }




  /////////////////////////////////////////////////////////
  //Calculate 2D slope 
  // in "cm" "cm" coordinates
  /////////////////////////////////////////////////////////
  double GeometryUtilities::Get2Dslope(double wireend,
				       double wirestart,
				       double timeend,
				       double timestart) const
  {
	
    return Get2Dslope(wireend-wirestart,timeend-timestart);
  
  }

  /////////////////////////////////////////////////////////
  //Calculate 2D slope 
  // in wire time coordinates coordinates
  /////////////////////////////////////////////////////////
  double GeometryUtilities::Get2Dslope(double dwire,
				       double dtime) const
  {
 
    //return omega;
 
    return tan(Get2Dangle(dwire,dtime))/fWireTimetoCmCm;

  }


  /////////////////////////////////////////////////////////
  //Calculate 2D angle 
  // in "cm" "cm" coordinates
  /////////////////////////////////////////////////////////
  double GeometryUtilities::Get2Dangle(double wireend,
				       double wirestart,
				       double timeend,
				       double timestart) const
  {

    return Get2Dangle(wireend-wirestart,timeend-timestart);
  
  }
  ////////////////////////////
  //Calculate 2D angle 
  // in "cm" "cm" coordinates
  ////////////////////////////
  double GeometryUtilities::Get2Dangle(double dwire,
				       double dtime) const
  {
 
    double BC,AC;
    double omega;
 
    BC = ((double)dwire)*fWiretoCm; // in cm
    AC = ((double)dtime)*fTimetoCm; //in cm 
    omega = std::asin(  AC/std::sqrt(pow(AC,2)+pow(BC,2)) );
    if(BC<0)  // for the time being. Will check if it works for AC<0
      { 
	if(AC>0){
	  omega= TMath::Pi()-std::abs(omega);  //
	}
	else if(AC<0){
	  omega=-TMath::Pi()+std::abs(omega);
	}
	else {
	  omega=TMath::Pi();
	}
      } 
    //return omega;
    return omega; //*fWirePitch/(fTimeTick*fDriftVelocity);

  }

  //////////////////////////////////////
  //Calculate 2D distance 
  // in "cm" "cm" coordinates
  ////////////////////////////////////////
  double GeometryUtilities::Get2DDistance(double wire1,
					  double time1,
					  double wire2,
					  double time2) const
  {
    
    return TMath::Sqrt( pow((wire1-wire2)*fWiretoCm,2)+pow((time1-time2)*fTimetoCm,2) );	
  
  }

  ////////////////////////////
  //Calculate 2D distance, using 2D angle 
  // in "cm" "cm" coordinates
  ////////////////////////////
  double GeometryUtilities::Get2DPitchDistance(double angle,
					       double inwire,
					       double wire) const
  {
    double radangle = TMath::Pi()*angle/180;
    if(std::cos(radangle)==0)
      return 9999;
    return std::abs((wire-inwire)/std::cos(radangle))*fWiretoCm; 
  }


  //----------------------------------------------------------------------------
  double GeometryUtilities::Get2DPitchDistanceWSlope(double slope,
						     double inwire,
						     double wire) const
  {
  
    return std::abs(wire-inwire)*TMath::Sqrt(1+slope*slope)*fWiretoCm; 
   
  }




  ///////////////////////////////////
  //Calculate wire,time coordinates of the Hit projection onto a line
  // 
  ///////////////////////////////////
  int GeometryUtilities::GetPointOnLine(double slope,
					double intercept,
					double wire1,
					double time1,
					double &wireout,
					double &timeout) const
  {
    double invslope=0;
      
    if(slope)	
      {
	invslope=-1./slope;
      }
  
    double ort_intercept=time1-invslope*(double)wire1;
    
    if((slope-invslope)!=0)
      wireout=(ort_intercept - intercept)/(slope-invslope); 
    else
      wireout=wire1;
    timeout=slope*wireout+intercept;   
    
    return 0;
  }
    
    
  ///////////////////////////////////
  //Calculate wire,time coordinates of the Hit projection onto a line
  // 
  ///////////////////////////////////    
  int GeometryUtilities::GetPointOnLine(double slope,
					double wirestart,
					double timestart,
					double wire1,
					double time1,
					double &wireout,
					double &timeout) const
  {
    double intercept=timestart-slope*(double)wirestart;
  
    return GetPointOnLine(slope,intercept,wire1,time1,wireout,timeout);
  }

  ///////////////////////////////////
  //Calculate wire,time coordinates of the Hit projection onto a line
  // 
  ///////////////////////////////////
  int GeometryUtilities::GetPointOnLineWSlopes(double slope,
					       double intercept,
					       double ort_intercept,
					       double &wireout,
					       double &timeout) const
  {
    double invslope=0;
  
    if(slope)	
	{
		invslope=-1./slope;
	}
    
    invslope*=fWireTimetoCmCm*fWireTimetoCmCm;
  	
    wireout=(ort_intercept - intercept)/(slope-invslope); 
    timeout=slope*wireout+intercept; 
  
    
    wireout/=fWiretoCm;
    timeout/=fTimetoCm;
    
    return 0;  
  }    

  ///////////////////////////////////
  //Find hit closest to wire,time coordinates
  // 
  ////////////////////////////////////////////////
  recob::Hit * GeometryUtilities::FindClosestHit(std::vector<art::Ptr< recob::Hit > > hitlist,
						 unsigned int wirein,
						 double timein) const
  {
  
   
    art::Ptr<recob::Hit> nearHit=FindClosestHitPtr(hitlist,wirein,timein);
//	min_length_from_start=dist_mod;
  
    return const_cast<recob::Hit *> (nearHit.get());    
  }
  
  
  //Find hit closest to wire,time coordinates
  // 
  ////////////////////////////////////////////////
  art::Ptr< recob::Hit > GeometryUtilities::FindClosestHitPtr(std::vector<art::Ptr< recob::Hit > > hitlist,
						 unsigned int wirein,
						 double timein) const
  {
  
    double min_length_from_start=99999;
    art::Ptr< recob::Hit > nearHit;
   
    unsigned int plane,tpc,wire,cstat;
   
   
    for(unsigned int ii=0; ii<hitlist.size();ii++){
      recob::Hit * theHit = const_cast<recob::Hit *>(hitlist[ii].get());
      double time = theHit->PeakTime() ;  
      GetPlaneAndTPC(theHit,plane,cstat,tpc,wire);
    
      double dist_mod=Get2DDistance(wirein,timein,wire,time);

      if(dist_mod<min_length_from_start){
	//wire_start[plane]=wire;
	//time_start[plane]=time;
	nearHit=(hitlist[ii]);
	min_length_from_start=dist_mod;
      }	

    } 
  
    return nearHit;    
  }
  
  
//     //Find hit closest to wire,time coordinates
//   // 
//   ////////////////////////////////////////////////
//   art::Ptr< recob::Hit > GeometryUtilities::FindClosestHitEvdPtr(std::vector<art::Ptr< recob::Hit > > hitlist,
// 						 unsigned int wirein,
// 						 double timein) const
//   {
//   
//     double min_length_from_start=99999;
//     art::Ptr< recob::Hit > nearHit;
//    
//     unsigned int plane,tpc,wire,cstat;
//    
//    
//     for(unsigned int ii=0; ii<hitlist.size();ii++){
//       recob::Hit * theHit = const_cast<recob::Hit *>(hitlist[ii].get());
//       double time = theHit->PeakTime() ;  
//       GetPlaneAndTPC(theHit,plane,cstat,tpc,wire);
//     
//       double dist_mod=Get2DDistance(wirein,timein,wire,time);
// 
//       if(dist_mod<min_length_from_start){
// 	//wire_start[plane]=wire;
// 	//time_start[plane]=time;
// 	nearHit=(hitlist[ii]);
// 	min_length_from_start=dist_mod;
//       }	
// 
//     } 
//   
//     return nearHit;    
//   }
//   
//   
//   
  
  
  
  //////////////////////////////////////////////////////////
  int GeometryUtilities::GetProjectedPoint(pxpoint p0, 
					   pxpoint p1, 
					   pxpoint &pN) const
  {
    //determine third plane number
    for(unsigned int i = 0; i < fNPlanes; ++i){
      if(i == p0.plane || i == p1.plane)
	continue;   
      pN.plane = i;
    }
  
    // Assuming there is no problem ( and we found the best pair that comes close in time )
    // we try to get the Y and Z coordinates for the start of the shower. 
    uint32_t chan1 = geom->PlaneWireToChannel(p0.plane,p0.w, 0);
    uint32_t chan2 = geom->PlaneWireToChannel(p1.plane,p1.w, 0);
 
 
    const double origin[3] = {0.};
    double pos[3];
    geom->Plane(p0.plane).LocalToWorld(origin, pos);
 
    double x=(p0.t-detp->TriggerOffset())*fTimetoCm+pos[0];
 
    double y,z;
    if(! geom->ChannelsIntersect(chan1,chan2,y,z) )
      return -1;
 
 
    pos[1]=y;
    pos[2]=z;
    pos[0]=x;
    
    pN=Get2DPointProjection(pos, pN.plane);
       
    return 0;  
  }


  //////////////////////////////////////////////////////////
  int GeometryUtilities::GetYZ(pxpoint p0,
			       pxpoint p1,
			       double* yz) const
  {
    double y,z;
  
    uint32_t chan1 = geom->PlaneWireToChannel(p0.plane, p0.w, 0);
    uint32_t chan2 = geom->PlaneWireToChannel(p1.plane, p1.w, 0);

    if(! geom->ChannelsIntersect(chan1,chan2,y,z) )
      return -1;
  
    yz[0]=y;
    yz[1]=z;
  
    return 0;
  }

  //////////////////////////////////////////////////////////////
  
   pxpoint GeometryUtilities::Get2DPointProjection(double *xyz, int plane) const{
  
    pxpoint pN(0,0,0);
    
    const double origin[3] = {0.}; 
    double pos[3];
    geom->Plane(plane).LocalToWorld(origin, pos);
    double drifttick=(xyz[0]/fDriftVelocity)*(1./fTimeTick);
      
    pos[1]=xyz[1];
    pos[2]=xyz[2];

    ///\todo: this should use the cryostat and tpc as well in the NearestWire method
    
    pN.w = geom->NearestWire(pos, plane);
    pN.t=drifttick-(pos[0]/fDriftVelocity)*(1./fTimeTick)+detp->TriggerOffset();  
    pN.plane=plane;
    
    return pN;
     
   }
  

  double GeometryUtilities::GetTimeTicks(double x, int plane) const{
  
   
    
    const double origin[3] = {0.}; 
    double pos[3];
    geom->Plane(plane).LocalToWorld(origin, pos);
    double drifttick=(x/fDriftVelocity)*(1./fTimeTick);
      
    
    return drifttick-(pos[0]/fDriftVelocity)*(1./fTimeTick)+detp->TriggerOffset();  
    
     
   }
  
  
  
  
  //----------------------------------------------------------------------
  // provide projected wire pitch for the view // copied from track.cxx and modified
  double GeometryUtilities::PitchInView(unsigned int plane,
					double phi,
					double theta) const
  {
    
    double dirs[3] = {0.};
    GetDirectionCosines(phi,theta,dirs); 
    
    art::ServiceHandle<geo::Geometry> geo;
    /// \todo switch to using new Geometry::WireAngleToVertical(geo::View_t) 
    /// \todo and Geometry::WirePitch(geo::View_t) methods
    double wirePitch   = 0.;
    double angleToVert = 0.;
   
    wirePitch = geo->WirePitch(0,1,plane);
    angleToVert = geo->Plane(plane).Wire(0).ThetaZ(false) - 0.5*TMath::Pi();
 
         
    //(sin(angleToVert),std::cos(angleToVert)) is the direction perpendicular to wire
    //fDir.front() is the direction of the track at the beginning of its trajectory
    double cosgamma = TMath::Abs(TMath::Sin(angleToVert)*dirs[1] + 
				      TMath::Cos(angleToVert)*dirs[2]);
   
    //   //std::cout << " ---- cosgamma: " << angleToVert*180/TMath::Pi() << " d's: " << dirs[1]
    //  << " " << dirs[2] << " ph,th " << phi << " " << theta << std::endl; 
    if(cosgamma < 1.e-5) 
      throw cet::exception("Track") << "cosgamma is basically 0, that can't be right";
    
    return wirePitch/cosgamma;
  }

  
  //////////////////////////////////////////////////
  void GeometryUtilities::GetDirectionCosines(double phi,
					      double theta,
					      double *dirs) const
  {
    theta*=(TMath::Pi()/180);
    phi*=(TMath::Pi()/180); // working on copies, it's ok.
    dirs[0]=TMath::Cos(theta)*TMath::Sin(phi);
    dirs[1]=TMath::Sin(theta);
    dirs[2]=TMath::Cos(theta)*TMath::Cos(phi);
   
  }


  //////////////////////////////////////////
  int GeometryUtilities::GetPlaneAndTPC(art::Ptr<recob::Hit> a,
					unsigned int &p,
					unsigned int &cs,
					unsigned int &t,
					unsigned int &w) const
  {
    p  = a->WireID().Plane;
    cs = a->WireID().Cryostat;
    t  = a->WireID().TPC;
    w  = a->WireID().Wire;
    
    return 0;
  }

  
   //////////////////////////////////////////
  int GeometryUtilities::GetPlaneAndTPC(recob::Hit*  a,
					unsigned int &p,
					unsigned int &cs,
					unsigned int &t,
					unsigned int &w) const
  {
    p  = a->WireID().Plane;
    cs = a->WireID().Cryostat;
    t  = a->WireID().TPC;
    w  = a->WireID().Wire;
    
    return 0;
  } 
  
  
  
  
  
  
 void GeometryUtilities::SelectLocalHitlist(std::vector< art::Ptr < recob::Hit> > hitlist, std::vector < art::Ptr<recob::Hit> > &hitlistlocal, double  wire_start,double time_start, double linearlimit,   double ortlimit, double lineslopetest)
{
  
  double locintercept=time_start-wire_start*lineslopetest;
  
     
  for(std::vector < art::Ptr < recob::Hit > >::const_iterator hitIter = hitlist.begin(); hitIter != hitlist.end();  hitIter++){
    	art::Ptr<recob::Hit> theHit = (*hitIter);
    	double time = theHit->PeakTime() ;  
    	unsigned int plane,cstat,tpc,wire;
	GetPlaneAndTPC(theHit,plane,cstat,tpc,wire);
	
	double wonline=wire,tonline=time;
	//gser.GetPointOnLine(lineslopetest,lineinterctest,wire,time,wonline,tonline);
	GetPointOnLine(lineslopetest,locintercept,wire,time,wonline,tonline);
	
	//calculate linear distance from start point and orthogonal distance from axis
	double lindist=Get2DDistance(wonline,tonline,wire_start,time_start);
	double ortdist=Get2DDistance(wire,time,wonline,tonline);
	
	////////std::cout << " w,t: " << wire << " " << time << " ws,ts " << wonline << " "<< tonline <<" "<< lindist << " " << ortdist << std::endl;
	
	if(lindist<linearlimit && ortdist<ortlimit)
	  { hitlistlocal.push_back(theHit);
	  //std::cout << " w,t: " << wire << " " << time << " calc time: " << wire*lineslopetest + locintercept  << " ws,ts " << wonline << " "<< tonline <<" "<< lindist << " " << ortdist  << " plane: " << plane << std::endl;
	  }
    
    
    }
    
} 
  
  
  


} // namespace
