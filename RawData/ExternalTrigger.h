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
#include <stdint.h>

namespace raw {

  class ExternalTrigger {
    public:
      ExternalTrigger(); // Default constructor

    private:

      unsigned int   fTrigID;
      uint64_t       fTrigTime;

#ifndef __GCCXML__

  public:

      ExternalTrigger(unsigned int trigid, uint64_t trigtime);

      // Set Methods
      void             SetTrigID(unsigned int i);
      void             SetTrigTime(uint64_t i);

      // Get Methods
      unsigned int     GetTrigID()          const;
      uint64_t         GetTrigTime()          const;
     
#endif
    };
}

#ifndef __GCCXML__

inline void           raw::ExternalTrigger::SetTrigID(unsigned int i)    { fTrigID = i;      }
inline void           raw::ExternalTrigger::SetTrigTime(uint64_t i)      { fTrigTime = i;    }
inline unsigned int   raw::ExternalTrigger::GetTrigID()           const  { return fTrigID;   }  
inline uint64_t        raw::ExternalTrigger::GetTrigTime()        const  { return fTrigTime; }  

#endif

#endif // EXTERNALTRIGGER_H

////////////////////////////////////////////////////////////////////////
