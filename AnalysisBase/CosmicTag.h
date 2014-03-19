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
    //virtual ~CosmicTag();
    ~CosmicTag();


    std::vector<float> endPt1; // x,y,z assuming t_0 = t_beam
    std::vector<float> endPt2; // x,y,z assuming t_0 = t_beam
    //    double flashTime; // this should go away -- it's sloppy
    float fCosmicScore; // 0 means not a cosmic, 1 means cosmic
    int fCosmicType; // 0 --> not a cosmic 
                    // 1 --> some part of the track/cluster is outside of the beam readout window
                    // 2 --> passes two Y or Z boundaries
                    // 3 --> passes an X boundary + another boundary
                    // add one for something that hits only one boundary

#ifndef __GCCXML__
  public:

    CosmicTag(
	      std::vector<float> ePt1,
	      std::vector<float> ePt2,
	      //      double flashTime,
	      float cScore,
	      int cType);

    CosmicTag(float cScore);



    friend std::ostream& operator << (std::ostream &o, CosmicTag const& a);

    //const float& DTwindow() const;
    float getXInteraction(float oldX, float xDrift, int tSample,  
			  float realTime, int tick ); 

    const float& CosmicScore() const;
    const int& CosmicType() const;
    
#endif
    //    ClassDef(CosmicTag, 1);
  };

}

#ifndef __GCCXML__

//inline const float& anab::CosmicTag::DTwindow()      const { return fDTwindow;      }
inline const float& anab::CosmicTag::CosmicScore() const {return fCosmicScore; }
inline const int& anab::CosmicTag::CosmicType() const {return fCosmicType; }


#endif

#endif //ANAB_COSMICTAG
