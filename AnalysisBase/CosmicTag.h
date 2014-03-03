////////////////////////////////////////////////////////////////////////////
// \version 0.0
//
// \brief Definition of data product to hold CosmicTag information
//
// \author lockwitz@fnal.gov
//
////////////////////////////////////////////////////////////////////////////
#ifndef ANAB_COSMICTAG_H
#define ANAB_COSMICTAG_H

#include <vector>
#include <iosfwd>
#include <iostream>
#include <iomanip>



namespace anab {

  class CosmicTag{
  public:
    
    CosmicTag();
    virtual ~CosmicTag();


    std::vector<float> endPt1; // x,y,z assuming t_0 = t_beam
    std::vector<float> endPt2; // x,y,z assuming t_0 = t_beam
    double flashTime;
    float cosmicScore; // 0 means not a cosmic, 1 means cosmic
    int cosmicType; // 


    

#ifndef __GCCXML__
  public:

    CosmicTag(
	      std::vector<float> endPt1,
	      std::vector<float> endPt2,
	      double flashTime,
	      float cosmicScore,
	      int cosmicType);




    friend std::ostream& operator << (std::ostream &o, CosmicTag const& a);

    //const float& DTwindow() const;
    float getXInteraction(float oldX, float xDrift, int tSample,  
			  float realTime, int tick ); 

    
#endif
    //    ClassDef(CosmicTag, 1);
  };

}

#ifndef __GCCXML__

//inline const float& anab::CosmicTag::DTwindow()      const { return fDTwindow;      }



#endif

#endif //ANAB_COSMICTAG
