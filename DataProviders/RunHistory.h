////////////////////////////////////////////////////////////////////////
// RunHistory.h
//
//  Data provider class for run history
//
// jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef RUNHISTORY_H
#define RUNHISTORY_H

#include <string>
#include <vector>
#include <map>

#include "fhiclcpp/ParameterSet.h"

///General LArSoft Utilities
namespace dataprov {

  namespace runhistory {

    enum RunType_t {
      kUnknownRunType=0,
      kProductionRun,
      kCommissioningRun,
      kTestRun,    
      kPedestalRun,
      kCalibrationRun,
      kNRunType
    };

    class SubRun {
    public:
      SubRun() : fTStart(0) {};
      ~SubRun() {};

      uint64_t TStart() { return fTStart; }
      void SetTStart(uint64_t t) { fTStart = t; }

    private:
      uint64_t fTStart;
    };
    
    class RunHistory {
    public:
      RunHistory();
      RunHistory(int runnum, fhicl::ParameterSet const& pset);
      virtual ~RunHistory();
      
      virtual bool   Configure(fhicl::ParameterSet const& pset);
      virtual bool   Update(uint64_t ts=0) = 0;
      
      int RunNumber() { return fRun; }
      int NSubruns() { return fNSubruns; }
      int RunType() { return fRunType; }
      virtual std::string RunTypeAsString();
      uint64_t TStart() { return fTStart; }
      uint64_t TStop()  { return fTStop; }
      uint64_t Duration()  { return fTStop-fTStart; }
      std::vector<std::string> Shifters() { return fShifter; } 

      std::vector<SubRun> SubRuns() { return fSubrun; }
      
      void SetNSubruns(int nsr) { fNSubruns = nsr;}
      void SetRunType(int rt) { fRunType = rt; }
      void SetDetId(int id) { fDetId = id; }
      void SetTStart(uint64_t t) { fTStart = t; }
      void SetTStop(uint64_t t) { fTStop = t; }
      void AddShifter(std::string sh) { fShifter.push_back(sh); }
      void SetShifters(std::vector<std::string> sh) { fShifter = sh; }
      void SetDetName(std::string dn) { fDetName = dn; }
      
    private:
    protected:
      int    fRun;
      int    fNSubruns;
      int    fRunType;
      int    fDetId;
      
      uint64_t  fTStart;
      uint64_t  fTStop;      

      std::vector<std::string> fShifter;
      std::string fDetName;

      std::vector<SubRun> fSubrun;
      
    }; // class RunHistory
  } // namespace runhistory
} //namespace dataprov
#endif // RunHistory_H
