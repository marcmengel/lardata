////////////////////////////////////////////////////////////////////////
// DatabaseUtil.h
//
// functions to talk to the Database
//
// andrzej.szelc@yale.edu
//
////////////////////////////////////////////////////////////////////////
#ifndef DATABASEUTIL_H
#define DATABASEUTIL_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include <libpq-fe.h>



///General LArSoft Utilities
namespace util{
    class DatabaseUtil {
    public:
      DatabaseUtil(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      ~DatabaseUtil();

      void   reconfigure(fhicl::ParameterSet const& pset);
      
      int GetLifetimeFromDB(int run,double &lftime_real);
      int GetTriggerOffsetFromDB(int run,double &T0_real);
      int GetTemperatureFromDB(int run,double &temp_real);
      int GetEfieldValuesFromDB(int run,std::vector<double> &efield);
      int GetPOTFromDB(int run,long double &POT);
      
      int SelectFieldByName(std::vector<std::string> &value,const char * field,const char * condition,const char * table);
      
      bool ToughErrorTreatment(){return fToughErrorTreatment;}
      bool ShouldConnect(){return fShouldConnect;}
      
    private:
      
      int SelectSingleFieldByQuery(std::vector<std::string> &value,const char * query);
      int Connect(int conn_wait=0);
      int DisConnect();
      char connection_str[200];
      
      PGconn *conn;       // database connection handle
      std::string fDBHostName;
      std::string fDBName;
      std::string fDBUser;
      std::string fTableName;
      int fPort;
      std::string fPassword;
      bool fToughErrorTreatment;
      bool fShouldConnect;
      
      
    }; // class DatabaseUtil
} //namespace util

DECLARE_ART_SERVICE(util::DatabaseUtil, LEGACY)
#endif
