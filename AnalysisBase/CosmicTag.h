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

  typedef enum cosmic_tag_id{
    kUnknown=-1,
    kGeometry_YY=1,
    kGeometry_YZ,
    kGeometry_ZZ,
    kGeometry_XX,
    kGeometry_XY,
    kGeometry_XZ,
    kOutsideDrift_Partial=100,
    kOutsideDrift_Complete,
    kFlash_BeamIncompatible=200,
    kFlash_Match
  } CosmicTagID_t;

  class CosmicTag{
  public:
    
    CosmicTag();

    std::vector<float> endPt1; // x,y,z assuming t_0 = t_beam
    std::vector<float> endPt2; // x,y,z assuming t_0 = t_beam
    float fCosmicScore; // 0 means not a cosmic, 1 means cosmic
    CosmicTagID_t fCosmicType; 

#ifndef __GCCXML__
  public:

    CosmicTag(std::vector<float> ePt1,
	      std::vector<float> ePt2,
	      //      double flashTime,
	      float cScore,
	      CosmicTagID_t cTypes);

    CosmicTag(float cScore);



    friend std::ostream& operator << (std::ostream &o, CosmicTag const& a);

    float getXInteraction(float oldX, float xDrift, int tSample,  
			  float realTime, int tick ); 

    const float& CosmicScore() const;
    const CosmicTagID_t& CosmicType() const;
    
#endif
  };

}

#ifndef __GCCXML__

inline const float& anab::CosmicTag::CosmicScore() const {return fCosmicScore; }
inline const CosmicTagID_t& anab::CosmicTag::CosmicType() const {return fCosmicType; }


#endif

#endif //ANAB_COSMICTAG
