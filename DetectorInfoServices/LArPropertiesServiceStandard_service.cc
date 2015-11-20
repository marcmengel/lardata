////////////////////////////////////////////////////////////////////////
//
//  LArProperties_plugin
//
////////////////////////////////////////////////////////////////////////
// Framework includes

// C++ language includes
#include <iostream>

// LArSoft includes
#include "Utilities/LArPropertiesServiceStandard.h"
//#include "SimpleTypesAndConstants/PhysicalConstants.h"

// ROOT includes
#include "TMath.h"

// Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/exception.h"
//-----------------------------------------------
util::LArPropertiesServiceStandard::LArPropertiesServiceStandard(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg)
{
  fProp.reset(new dataprov::LArPropertiesStandard());

  this->reconfigure(pset);
  reg.sPreBeginRun.watch(this, &LArPropertiesServiceStandard::preBeginRun);
}

//----------------------------------------------
void util::LArPropertiesServiceStandard::preBeginRun(const art::Run& run)
{
  fProp->Update(run.id().run());
}



//------------------------------------------------
/// \todo these values should eventually come from a database
void util::LArPropertiesServiceStandard::reconfigure(fhicl::ParameterSet const& pset)
{
  fProp->Configure(pset);  
  return;
}

//------------------------------------------------
namespace util{
 
  DEFINE_ART_SERVICE_INTERFACE_IMPL(util::LArPropertiesServiceStandard, util::LArPropertiesService)

} // namespace util
