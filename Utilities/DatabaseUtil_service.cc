////////////////////////////////////////////////////////////////////////
//
//  DatabaseUtil_plugin
//
////////////////////////////////////////////////////////////////////////
// Framework includes

// C++ language includes
#include <iostream>
#include <fstream>
//#include <libpq-fe.h>

// LArSoft includes
#include "Utilities/DatabaseUtil.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/exception.h"

//-----------------------------------------------
util::DatabaseUtil::DatabaseUtil(fhicl::ParameterSet const& pset, art::ActivityRegistry & /* reg */)
{
  conn = NULL;
  this->reconfigure(pset);
  fChannelMap.clear();
  fChannelReverseMap.clear();
}

//------------------------------------------------
util::DatabaseUtil::~DatabaseUtil() 
{

}

//----------------------------------------------
int util::DatabaseUtil::Connect(int conn_wait)
{
  if(!fShouldConnect)
    return -1;
  
  if(conn_wait)
    sleep(conn_wait);
    
  conn = PQconnectdb(connection_str);
  if (PQstatus(conn) == CONNECTION_BAD) {
    mf::LogWarning("DatabaseUtil") << "Connection to database failed, "<<PQerrorMessage(conn)<<"\n";
    if( ( strstr(PQerrorMessage(conn),"remaining connection slots are reserved")!=NULL || 
      strstr(PQerrorMessage(conn),"sorry, too many clients already")!=NULL )
      && conn_wait<20 ) {
	conn_wait+=2;
	mf::LogWarning("DatabaseUtil") << "retrying connection after " << conn_wait << " seconds \n";
	return this->Connect(conn_wait);
      }
   if(fToughErrorTreatment)
       throw cet::exception("DataBaseUtil") << " DB connection failed\n";
   
  } else {
    LOG_DEBUG("DatabaseUtil")<<"Connected OK\n";
    return 1;
  }
  return -1;
}


int util::DatabaseUtil::DisConnect()
{
  if(!fShouldConnect)
    return -1;
  //close connection
  LOG_DEBUG("DatabaseUtil")<<"Closing Connection \n";
  PQfinish(conn);
  return 1;
}



//------------------------------------------------
void util::DatabaseUtil::reconfigure(fhicl::ParameterSet const& pset)
{
  fDBHostName            = pset.get< std::string >("DBHostName"          );
  fDBName      		 = pset.get< std::string >("DBName"     );
  fDBUser 		 = pset.get< std::string >("DBUser");
  fTableName 		 = pset.get< std::string >("TableName");
  fPort      		 = pset.get< int >("Port"     );
  fPassword 		 = " ";
  fToughErrorTreatment   = pset.get< bool >("ToughErrorTreatment");
  fShouldConnect   	 = pset.get< bool >("ShouldConnect");
  
  // constructor decides if initialized value is a path or an environment variable
  std::string passfname;
  cet::search_path sp("FW_SEARCH_PATH");
  sp.find_file(pset.get< std::string >("PassFileName"), passfname);
    
  std::ifstream in(passfname.c_str());
  if(in.is_open())   {
    std::getline(in, fPassword);
    in.close();
  }
   
   sprintf(connection_str,"host=%s dbname=%s user=%s port=%d password=%s",fDBHostName.c_str(),fDBName.c_str(),fDBUser.c_str(),fPort,fPassword.c_str());
   
  return;
}




int util::DatabaseUtil::SelectSingleFieldByQuery(std::vector<std::string> &value,const char * query)
{
  PGresult *result;
  char * string_val;
  
  if(this->Connect()==-1)  {
    if(fShouldConnect)
      mf::LogWarning("DatabaseUtil")<< "DB Connection error \n";
    else
      mf::LogInfo("DatabaseUtil")<< "Not connecting to DB by choice. \n";
   return -1;
  }
    
  result = PQexec(conn, query);

  if (!result) {
     mf::LogInfo("DatabaseUtil")<< "PQexec command failed, no error code\n";
    return -1;
  } 
  else if(PQresultStatus(result)!=PGRES_TUPLES_OK) {
    if(PQresultStatus(result)==PGRES_COMMAND_OK) 
  	LOG_DEBUG("DatabaseUtil")<<"Command executed OK, "<< PQcmdTuples(result) <<" rows affected\n";
    else
	mf::LogWarning("DatabaseUtil")<<"Command failed with code "
	  <<PQresStatus(PQresultStatus(result)) <<", error message "
	  <<PQresultErrorMessage(result)<<"\n";
		 
    PQclear(result);	
    this->DisConnect();
    return -1;
  }
  else {
  //  mf::LogInfo("DatabaseUtil")<<"Query may have returned data\n";
  //  mf::LogInfo("DatabaseUtil")<<"Number of rows returned: "<<PQntuples(result)
  //   <<", fields: "<<PQnfields(result)<<" \n";
    
    if(PQntuples(result)>=1){
	for(int i=0;i<PQntuples(result);i++)
	  {
	  string_val=PQgetvalue(result,i,0);
	  value.push_back(string_val);
	  LOG_DEBUG("DatabaseUtil")<<" extracted value: "<<value[i] << "\n";
	  }
	PQclear(result);
	this->DisConnect();
	return 0;
    }
    else {
     mf::LogWarning("DatabaseUtil")<<"wrong number of rows returned:"<<PQntuples(result)<<"\n";
      PQclear(result);
      this->DisConnect();
      return -1;
    }	
  }
  
  
}



int util::DatabaseUtil::GetTemperatureFromDB(int run,double &temp_real)
{
  std::vector<std::string> retvalue;
  char cond[30];
  sprintf(cond,"run = %d",run);
  int err=SelectFieldByName(retvalue,"temp",cond,fTableName.c_str());
  
  if(err!=-1 && retvalue.size()==1){
    char * endstr;
    temp_real=std::strtod(retvalue[0].c_str(),&endstr); 
    return 0; 
    }

return -1;
 
  
}




int util::DatabaseUtil::GetEfieldValuesFromDB(int run,std::vector<double> &efield)
{
  
  std::vector<std::string> retvalue;
    
  char query[200];
  sprintf(query,"SELECT EFbet FROM EField,%s WHERE Efield.FID = %s.FID AND run = %d ORDER BY planegap",fTableName.c_str(),fTableName.c_str(),run);
  int err=SelectSingleFieldByQuery(retvalue,query);
  
  if(err!=-1 && retvalue.size()>=1){
    efield.clear();    //clear value before setting new values
    for(unsigned int i=0;i<retvalue.size();i++) {
      char * endstr;
      efield.push_back(std::strtod(retvalue[i].c_str(),&endstr)); 
      }
    return 0; 
    }

 return -1;
    
}
      


int util::DatabaseUtil::SelectFieldByName(std::vector<std::string> &value,
					  const char * field,
					  const char * condition,
					  const char * table)	{
  
  char query[100];
 sprintf(query,"SELECT %s FROM %s WHERE %s",field, table, condition);
  
 return SelectSingleFieldByQuery(value,query);
    
}
      







int util::DatabaseUtil::GetLifetimeFromDB(int run,double &lftime_real) {

//  char query[100];
//  sprintf(query,"SELECT tau FROM argoneut_test WHERE run = %d",run);

  std::vector<std::string> retvalue;
  char cond[30];
  sprintf(cond,"run = %d",run);
  int err=SelectFieldByName(retvalue,"tau",cond,fTableName.c_str());
  
  if(err!=-1 && retvalue.size()==1){
   char * endstr;
   lftime_real=std::strtod(retvalue[0].c_str(),&endstr); 
   return 0; 
    }


return -1;

}

int util::DatabaseUtil::GetTriggerOffsetFromDB(int run,double &T0_real) {

//  char query[100];
//  sprintf(query,"SELECT tau FROM argoneut_test WHERE run = %d",run);

  std::vector<std::string> retvalue;
  char cond[30];
  sprintf(cond,"run = %d",run);
  int err=SelectFieldByName(retvalue,"T0",cond,fTableName.c_str());
  
  if(err!=-1 && retvalue.size()==1){
   char * endstr;
   T0_real=std::strtod(retvalue[0].c_str(),&endstr); 
   return 0; 
    }


return -1;

}


int util::DatabaseUtil::GetPOTFromDB(int run,long double &POT) {

//  char query[100];
//  sprintf(query,"SELECT tau FROM argoneut_test WHERE run = %d",run);

  std::vector<std::string> retvalue;
  char cond[30];
  sprintf(cond,"run = %d",run);
  int err=SelectFieldByName(retvalue,"pot",cond,fTableName.c_str());
  
  if(err!=-1 && retvalue.size()==1){
    char * endstr;
   POT=std::strtold(retvalue[0].c_str(),&endstr); 
   return 0; 
    }


return -1;

}

namespace util {


  void DatabaseUtil::LoadUBChannelMap( bool get_from_db ) {

    if ( get_from_db==false && fChannelMap.size()>0 ) {
      // used prevously grabbed data to avoid repeated call to database.
      return;
    }

    if ( conn==NULL )
      Connect( 0 );

    if(PQstatus(conn)!=CONNECTION_OK) {
      mf::LogError("") << __PRETTY_FUNCTION__ << ": Couldn't open connection to postgresql interface";
      PQfinish(conn);
      throw art::Exception( art::errors::FileReadError )
        << "Failed to get channel map from DB." << std::endl;
    }
    
    fChannelMap.clear();
    fChannelReverseMap.clear();

    PGresult *res  = PQexec(conn, "BEGIN");    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) { 
      mf::LogError("")<< "postgresql BEGIN failed";
      PQclear(res);
      PQfinish(conn);
      throw art::Exception( art::errors::FileReadError )
        << "postgresql BEGIN failed." << std::endl;
    }
    
    // Jason St. John's updated call to versioned database.
    // getmap(data_taking_timestamp timestamp DEFAULT now(), 
    //        swizzling_timestamp timestamp DEFAULT now()    )
    // Returns rows of: crate, slot, fem_channel, larsoft_channel 
    // Both arguments are optional, or can be passed their default of now(), or can be passed an explicit timestamp:
    // Example: "SELECT getmap(TIMESTAMP '2015-08-01 12:34:56')"
    PQclear(res);
    res = PQexec(conn, "SELECT get_map()");

    if ((!res) || (PQresultStatus(res) != PGRES_TUPLES_OK))
      {
	mf::LogError("")<< "SELECT command did not return tuples properly";
        PQclear(res);
        PQfinish(conn);
        throw art::Exception( art::errors::FileReadError )
          << "postgresql SELECT failed." << std::endl;
      }

    int num_records=PQntuples(res);
    for (int i=0;i<num_records;i++) {
      int crate_id     =  atoi(PQgetvalue(res, i, 0));
      int slot         = atoi(PQgetvalue(res, i, 1));
      int boardChan   = atoi(PQgetvalue(res, i, 2));
      int larsoft_chan = atoi(PQgetvalue(res, i, 3));

      UBDaqID daq_id(crate_id,slot,boardChan);
      std::pair<UBDaqID, UBLArSoftCh_t> p(daq_id,larsoft_chan);
      
      if ( fChannelMap.find(daq_id) != fChannelMap.end() ){
	std::cout << __PRETTY_FUNCTION__ << ": ";
        std::cout << "Multiple entries!" << std::endl;
        mf::LogWarning("")<< "Multiple DB entries for same (crate,card,channel). "<<std::endl
			  << "Redefining (crate,card,channel)=>id link ("
			  << daq_id.crate<<", "<< daq_id.card<<", "<< daq_id.channel<<")=>"
			  << fChannelMap.find(daq_id)->second;
      }
       
      fChannelMap.insert( p );
      fChannelReverseMap.insert( std::pair< UBLArSoftCh_t, UBDaqID >( larsoft_chan, daq_id ) );
    }

  }// end of LoadUBChannelMap

  const UBChannelMap_t& DatabaseUtil::GetUBChannelMap( bool get_from_db ) {
    LoadUBChannelMap( get_from_db );
    return fChannelMap;
  }

  const UBChannelReverseMap_t& DatabaseUtil::GetUBChannelReverseMap( bool get_from_db ) {
    LoadUBChannelMap( get_from_db );
    return fChannelReverseMap;
  }
  
}

namespace util{
 
  DEFINE_ART_SERVICE(DatabaseUtil)

} // namespace util
