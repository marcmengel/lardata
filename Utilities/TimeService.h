////////////////////////////////////////////////////////////////////////
//
// TimeService.h
//
////////////////////////////////////////////////////////////////////////
#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Persistency/Common/Ptr.h"
#include "art/Persistency/Common/PtrVector.h"
#include "art/Persistency/RootDB/SQLite3Wrapper.h"

#include "RawData/TriggerData.h"
#include "Utilities/DatabaseUtil.h"
#include "SimpleTimeService.h"

namespace util{

  enum InheritConfigType_t {
    kG4RefTime=0,
    kTriggerOffsetTPC,
    kFramePeriod,
    kClockSpeedTPC,
    kClockSpeedOptical,
    kClockSpeedTrigger,
    kClockSpeedExternal,
    kDefaultTrigTime,
    kDefaultBeamTime,
    kInheritConfigTypeMax
  };

  class TimeService : public SimpleTimeService {

  public:
    TimeService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    ~TimeService(){};

  public:

    /// Override of base class function ... implement DB status check
    virtual double TriggerOffsetTPC() const
    { 
      if(!fAlreadyReadFromDB) 
	CheckDBStatus(); 
      if (fTriggerOffsetTPC<0)
	return fTriggerOffsetTPC; 
      else
	return -fTriggerOffsetTPC/fTPCClock.Frequency(); //convert ticks to us
    }

    //
    // All following functions are not for users to execute (but I believe they have to be public)
    //

    /// Re-configure the service module
    void reconfigure(fhicl::ParameterSet const& pset);

    /// Function to be executed @ run boundary
    void preBeginRun(art::Run const& run);

    /// Function to be executed @ event boundary
    void preProcessEvent(const art::Event& evt);

    /// Function to be executed @ file open
    void postOpenFile(const std::string& filename);

    /// Function to report variable contents for cout-debugging
    void debugReport() const;

  protected:

    /// Internal function to apply loaded parameters to member attributes
    void ApplyParams();

    /// Internal function used to search for the right configuration set in the data file
    bool IsRightConfig(const fhicl::ParameterSet& ps) const;

    /// Internal function used to check DB status (inherited from DetectorProperties)
    void CheckDBStatus() const;

  protected:

    std::vector<std::string> fConfigName;

    std::vector<double>      fConfigValue;

    bool fInheritClockConfig;

    bool fAlreadyReadFromDB;

    std::string fTrigModuleName;

  }; // class TimeService

} //namespace utils

DECLARE_ART_SERVICE(util::TimeService, LEGACY)

#endif 
