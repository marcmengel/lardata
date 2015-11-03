////////////////////////////////////////////////////////////////////////
// \file ILArProperties.h
//
// \brief pure virtual base interface for run history
//
// \author jpaley@fnal.gov
// 
////////////////////////////////////////////////////////////////////////
#ifndef DATAPROV_IRUNHISTORY_H
#define DATAPROV_IRUNHISTORY_H

///General LArSoft Utilities
namespace dataprov {

  enum RunType_t {
    kUnknownRunType=0,
    kProductionRun,
    kCommissioningRun,
    kTestRun,    
    kPedestalRun,
    kCalibrationRun,
    kNRunType
  };
  
  class ISubRun {
  public:
    virtual ~ISubRun() = default;
    
    virtual uint64_t TStart() const = 0;
  };
  
  class IRunHistory {
  public:
    virtual ~IRunHistory() = default;

    virtual bool Update() = 0;
    
    virtual int RunNumber() const = 0;
    virtual int NSubruns() const = 0; 
    virtual int RunType() const = 0; 
    virtual std::string RunTypeAsString() const = 0;
    virtual uint64_t TStart() const = 0;
    virtual uint64_t TStop()  const = 0;
    virtual uint64_t Duration() const = 0;
    
  }; // class IRunHistory
} //namespace dataprov
#endif // RunHistory_H
