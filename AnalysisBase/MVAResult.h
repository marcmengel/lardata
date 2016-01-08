#ifndef MVARESULT_H
#define MVARESULT_H

#include <vector>
#include <map>
#include <string>

namespace anab {

struct MVAResult {

  float evalRatio;
  float concentration;
  float coreHaloRatio;
  float conicalness;
  float dEdxStart;
  float dEdxEnd;
  float dEdxPenultimate;
  float nSpacePoints;
  unsigned int trackID;

  friend bool           operator <  (const MVAResult & a, const MVAResult & b);

  std::map<std::string,double> mvaOutput; 

};

bool operator < (const MVAResult & a, const MVAResult & b)
  {
    return a.nSpacePoints<b.nSpacePoints;
  } 


}

#endif
