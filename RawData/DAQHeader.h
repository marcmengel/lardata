////////////////////////////////////////////////////////////////////////
// $Id: DAQHeader.h,v 1.7 2009/02/19 23:07:30 soderber Exp $
//
// Definition of basic DAQ header information
//
// brebel@fnal.gov
//
// -modified DAQHeader class to save all information available in 
//  binary version of DAQ480 software.  - Mitch Soderberg 2/19/09  
//
////////////////////////////////////////////////////////////////////////

#ifndef DAQHEADER_H
#define DAQHEADER_H

#include <vector>
#include <iosfwd>
#include <time.h>
#include <stdint.h>

namespace raw {

  class DAQHeader {
    public:
      DAQHeader(); // Default constructor

    private:

      unsigned int   fStatus;
      int            fFixed;
      unsigned short fFormat;
      unsigned short fSoftware;
      unsigned short fRun;
      unsigned short fEvent;
      time_t         fTime;
      short          fSpare;
      uint32_t       fNchan;

#ifndef __GCCXML__

  public:

      DAQHeader(unsigned int status); // Ascii DAQ constructor
      DAQHeader(unsigned int status,  // Binary DAQ constructor
		int fixed,
		unsigned short format,
		unsigned short software,
		unsigned short run,
		unsigned short event,
		time_t time,
		short spare,
		uint32_t nchan);

      // Set Methods
      void             SetStatus(unsigned int i);
      void             SetFixedWord(int i);
      void             SetFileFormat(unsigned short i);
      void             SetSoftwareVersion(unsigned short i);
      void             SetRun(unsigned short i);
      void             SetEvent(unsigned short i);
      void             SetTimeStamp(time_t t);
      void             SetSpareWord(short s);
      void             SetNChannels(uint32_t i);

      // Get Methods
      unsigned int     GetStatus()          const;
      int              GetFixedWord()       const;
      unsigned short   GetFileFormat()      const;
      unsigned short   GetSoftwareVersion() const;
      unsigned short   GetRun()             const;
      unsigned short   GetEvent()           const;
      time_t           GetTimeStamp()       const;
      short            GetSpareWord()       const;
      uint32_t         GetNChannels()       const;
     
#endif
    };
}

#ifndef __GCCXML__

inline void           raw::DAQHeader::SetStatus(unsigned int i)             { fStatus = i;      }
inline void           raw::DAQHeader::SetFixedWord(int i)                   { fFixed = i;       }
inline void           raw::DAQHeader::SetFileFormat(unsigned short i)       { fFormat = i;      }
inline void           raw::DAQHeader::SetSoftwareVersion(unsigned short i)  { fSoftware = i;    }
inline void           raw::DAQHeader::SetRun(unsigned short i)              { fRun = i;         }
inline void           raw::DAQHeader::SetEvent(unsigned short i)            { fEvent = i;       }
inline void           raw::DAQHeader::SetTimeStamp(time_t t)                { fTime = t;        }
inline void           raw::DAQHeader::SetSpareWord(short s)                 { fSpare = s;       }
inline void           raw::DAQHeader::SetNChannels(uint32_t i)              { fNchan = i;       }
inline unsigned int   raw::DAQHeader::GetStatus()          const            { return fStatus;   }  
inline int            raw::DAQHeader::GetFixedWord()       const 	    { return fFixed;    }   
inline unsigned short raw::DAQHeader::GetFileFormat()      const 	    { return fFormat;   }  
inline unsigned short raw::DAQHeader::GetSoftwareVersion() const 	    { return fSoftware; }
inline unsigned short raw::DAQHeader::GetRun()             const 	    { return fRun;      }     
inline unsigned short raw::DAQHeader::GetEvent()           const 	    { return fEvent;    }   
inline time_t         raw::DAQHeader::GetTimeStamp()       const 	    { return fTime;     }    
inline short          raw::DAQHeader::GetSpareWord()       const 	    { return fSpare;    }   
inline uint32_t       raw::DAQHeader::GetNChannels()       const 	    { return fNchan;    }   

#endif

#endif // DAQHEADER_H

////////////////////////////////////////////////////////////////////////
