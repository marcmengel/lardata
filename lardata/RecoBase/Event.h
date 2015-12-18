////////////////////////////////////////////////////////////////////////////
// \version $Id: Event.h,v 1.4 2010/06/10 16:21:31 antonm Exp $
//
// \brief Definition of event object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#ifndef RB_EVENT_H
#define RB_EVENT_H

#ifndef __GCCXML__

#include <iosfwd>
#include <vector>

#endif

namespace recob {

  class Vertex; // Forward declaration.

  class Event  {

  public:
    Event(); // Needed to hide constexpr from GCCXML.

  private:

    int                  fID;       ///< id for this event

#ifndef __GCCXML__

  public:
    explicit Event(int id);

    double               Energy()        const;
    double               SigmaEnergy()   const;
    const recob::Vertex* PrimaryVertex(std::vector<const recob::Vertex*>& vtxs) const;
    int                  ID()            const;

    friend std::ostream& operator << (std::ostream& o, const Event & a);
    friend bool          operator <  (const Event & a, const Event & b);

#endif

  };
}

#ifndef __GCCXML__

inline int  recob::Event::ID() const { return fID; }

#endif

#endif // RB_EVENT_H
