
#ifndef MCWIRECOLLECTION_H
#define MCWIRECOLLECTION_H

// C++ includes
#include "MCWire.h"

namespace sim {
  
  class MCWireCollection : public std::vector<sim::MCWire> {

  public:

    /// Default ctor
    MCWireCollection(const unsigned int ch=sim::kINVALID_UINT) 
    {
      Reset();
      fChannel    = ch;
    }

    void Reset()
    {
      std::vector<sim::MCWire>::clear();
      fChannel  = sim::kINVALID_UINT;
    }

  private:

    unsigned int fChannel;

#ifndef __GCCXML__

  public:

    /// Getter for channel number
    unsigned int Channel() const { return fChannel; }

    /// For sorting
    inline bool operator< ( const MCWireCollection& rhs ) const { return fChannel < rhs.fChannel; }

    /// wrapper for push_back
    inline void push_back(const MCWire& wire)
    {

      bool sort = (!empty() && wire < (*rbegin()));

      std::vector<sim::MCWire>::push_back(wire);
      
      if(sort) std::sort(begin(),end());
      
    }
#endif
  };
}

// Define a pointer comparison
#ifndef __GCCXML__
namespace std {
  template <>
  class less<sim::MCWireCollection*>
  {
  public:
    bool operator()( const sim::MCWireCollection* lhs, const sim::MCWireCollection* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif 
