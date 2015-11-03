////////////////////////////////////////////////////////////////////////
//
//  RunHistory
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
// Framework includes

// C++ language includes
#include <iostream>

// LArSoft includes
#include "DataProviders/RunHistory.h"

// ROOT includes
#include "TMath.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/exception.h"

namespace dataprov {
  //-----------------------------------------------
  RunHistory::RunHistory() :
    fRun(-1), fNSubruns(0), fRunType(kUnknownRunType)
  {
  }
  
  //-----------------------------------------------
  RunHistory::RunHistory(int run)
  {
    fRun=run;
  }
  
  //------------------------------------------------
  RunHistory::~RunHistory()
  {
  }

  //------------------------------------------------
  bool RunHistory::Update(uint64_t ts) 
  {
    if (ts == 0) return false;

    return true;
  }

  //------------------------------------------------
  std::string RunHistory::RunTypeAsString() const
  {
    switch(fRunType) {
    case(kProductionRun):
      return std::string("Production");
    case(kCommissioningRun):
      return std::string("Commissioning");
    case(kTestRun):
      return std::string("Test");
    case(kPedestalRun):
      return std::string("Pedestal");
    case(kCalibrationRun):
      return std::string("Calibration");
    case(kUnknownRunType):
    default:
      return std::string("Uknown");
    }
  }
}
