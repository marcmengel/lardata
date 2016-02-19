#ifndef TRIGGERDATA_CXX
#define TRIGGERDATA_CXX

#include "TriggerData.h"

namespace raw {

  //****************************************************
  bool Trigger::Triggered(const unsigned char bit) const
  //****************************************************
  {

    if(bit>32) {
      throw std::invalid_argument("\n\nCannot access bit higher than 32!\n");
      std::exit(1);
    }

    return ( (fTriggerBits >> bit) & 0x1);

  }

}

#endif
