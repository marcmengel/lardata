#ifndef MVAPIDRESULT_H
#define MVAPIDRESULT_H

#include <vector>
#include <map>
#include <string>

namespace anab {

struct MVAPIDResult {

  float evalRatio;
  float concentration;
  float coreHaloRatio;
  float conicalness;
  float dEdxStart;
  float dEdxEnd;
  float dEdxPenultimate;
  float nSpacePoints;
  unsigned int trackID;

  friend bool           operator <  (const MVAPIDResult & a, const MVAPIDResult & b);

  std::map<std::string,double> mvaOutput; 

};

bool operator < (const MVAPIDResult & a, const MVAPIDResult & b)
  {
    return a.nSpacePoints<b.nSpacePoints;
  } 


}

#endif
