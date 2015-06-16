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

  class UBDaqID {
  public:
  UBDaqID() : crate(-1), card(-1), channel(-1) {};
  UBDaqID( int _crate, int _card, int _channel ) :    
    crate(_crate), card(_card), channel(_channel) {};
    ~UBDaqID() {};
    
    int crate;
    int card;
    int channel;

    const bool operator<(const UBDaqID& rhs) const {
      bool is_less=false;
      if (this->crate   == rhs.crate &&
    	  this->card    == rhs.card  &&
    	  this->channel <  rhs.channel) is_less=true;
      else if (this->crate == rhs.crate &&
    	       this->card  <  rhs.card) is_less=true;
      else if (this->crate < rhs.crate) is_less=true;
      return is_less;
    }
    
  };

  
  typedef int UBLArSoftCh_t;

  typedef std::map< UBDaqID, UBLArSoftCh_t > UBChannelMap_t;


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
      UBChannelMap_t GetUBChannelMap();

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
