////////////////////////////////////////////////////////////////////////
// $Id: ExternalTrigger.h 
//
// 
//
//
////////////////////////////////////////////////////////////////////////

#ifndef EXTERNALTRIGGER_H
#define EXTERNALTRIGGER_H

#include <vector>
#include <iosfwd>
#include <time.h>

namespace raw {

  class ExternalTrigger {
    public:
      ExternalTrigger(); // Default constructor

    private:

      unsigned int   fTrigID;
      unsigned int   fTrigTime;

#ifndef __GCCXML__

  public:

      ExternalTrigger(unsigned int trigid, unsigned int trigtime);

      // Set Methods
      void             SetTrigID(unsigned int i);
      void             SetTrigTime(unsigned int i);

      // Get Methods
      unsigned int     GetTrigID()          const;
      unsigned int     GetTrigTime()          const;
     
#endif
    };
}

#ifndef __GCCXML__

inline void           raw::ExternalTrigger::SetTrigID(unsigned int i)    { fTrigID = i;      }
inline void           raw::ExternalTrigger::SetTrigTime(unsigned int i)  { fTrigTime = i;    }
inline unsigned int   raw::ExternalTrigger::GetTrigID()           const  { return fTrigID;   }  
inline unsigned int   raw::ExternalTrigger::GetTrigTime()         const  { return fTrigTime; }  

#endif

#endif // EXTERNALTRIGGER_H

////////////////////////////////////////////////////////////////////////
