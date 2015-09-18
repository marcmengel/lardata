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

namespace rh = dataprov::runhistory;

//-----------------------------------------------
rh::RunHistory::RunHistory() :
  fRun(-1), fNSubruns(0), fRunType(rh::kUnknownRunType)
{
}

//-----------------------------------------------
rh::RunHistory::RunHistory(int run, fhicl::ParameterSet const& pset)
{
  fRun=run;
  this->Configure(pset);
}

//------------------------------------------------
rh::RunHistory::~RunHistory()
{
}

//------------------------------------------------
bool rh::RunHistory::Configure(fhicl::ParameterSet const& pset)
{  

  return true;
}

//------------------------------------------------
bool rh::RunHistory::Update(uint64_t ts) 
{
  if (ts == 0) return false;

  return true;
}

//------------------------------------------------
std::string rh::RunHistory::RunTypeAsString()
{
  switch(fRunType) {
  case(rh::kProductionRun):
    return std::string("Production");
  case(rh::kCommissioningRun):
    return std::string("Commissioning");
  case(rh::kTestRun):
    return std::string("Test");
  case(rh::kPedestalRun):
    return std::string("Pedestal");
  case(rh::kCalibrationRun):
    return std::string("Calibration");
  case(rh::kUnknownRunType):
  default:
    return std::string("Uknown");
  }
}

