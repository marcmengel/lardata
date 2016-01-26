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

  typedef int64_t TriggerTimeStamp_t; ///< type of trigger time stamp

  class ExternalTrigger {
    public:
      ExternalTrigger(); // Default constructor

    private:

      unsigned int       fTrigID;
      TriggerTimeStamp_t fTrigTime;

#ifndef __GCCXML__

  public:

      ExternalTrigger(unsigned int trigid, TriggerTimeStamp_t trigtime);

      // Set Methods
      void             SetTrigID(unsigned int i);
      void             SetTrigTime(TriggerTimeStamp_t i);

      // Get Methods
      unsigned int       GetTrigID()   const;
      TriggerTimeStamp_t GetTrigTime() const;
     
#endif
    };

}

#ifndef __GCCXML__

namespace raw{
  inline void               ExternalTrigger::SetTrigID(unsigned int i)         { fTrigID = i;      }
  inline void               ExternalTrigger::SetTrigTime(TriggerTimeStamp_t i) { fTrigTime = i;    }
  inline unsigned int       ExternalTrigger::GetTrigID()    const  { return fTrigID;   }  
  inline TriggerTimeStamp_t ExternalTrigger::GetTrigTime()  const  { return fTrigTime; }
}
#endif

#endif // EXTERNALTRIGGER_H

////////////////////////////////////////////////////////////////////////
