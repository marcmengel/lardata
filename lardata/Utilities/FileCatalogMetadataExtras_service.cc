////////////////////////////////////////////////////////////////////////
//
//  Name: FileCatalogMetadataExtras_service.cc
//
////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include "lardata/Utilities/FileCatalogMetadataExtras.h"
#include "art/Framework/Services/System/FileCatalogMetadata.h"
#include "art/Utilities/OutputFileInfo.h"
#include "art/Framework/IO/Root/RootDB/SQLite3Wrapper.h"
#include "art/Framework/IO/Root/RootDB/SQLErrMsg.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "TROOT.h"
#include "TFile.h"

//--------------------------------------------------------------------
// Constructor.
util::FileCatalogMetadataExtras::FileCatalogMetadataExtras(fhicl::ParameterSet const& pset, 
							   art::ActivityRegistry &reg) :
  fGeneratePerFileMetadata(false),
  fRenameOverwrite(false),
  fOutputFileCount(0)
{
  reconfigure(pset);

  // Register for callbacks.

  reg.sPostBeginJob.watch(this, &FileCatalogMetadataExtras::postBeginJob);
  reg.sPostEndJob.watch(this, &FileCatalogMetadataExtras::postEndJob);
  reg.sPostOpenFile.watch(this, &FileCatalogMetadataExtras::postOpenFile);
  reg.sPostCloseFile.watch(this, &FileCatalogMetadataExtras::postCloseFile);
  reg.sPreProcessEvent.watch(this, &FileCatalogMetadataExtras::preEvent);
  reg.sPostProcessEvent.watch(this, &FileCatalogMetadataExtras::postEvent);
  reg.sPostCloseOutputFile.watch(this, &FileCatalogMetadataExtras::postCloseOutputFile);
}


//--------------------------------------------------------------------
// Destructor.
util::FileCatalogMetadataExtras::~FileCatalogMetadataExtras() 
{
  // Shouldn't really be necessary to call checkOutputFiles, as we can
  // catch final closed files via postEndJob callback.  But do it just
  // for extra safety, and can't do any harm.

  checkOutputFiles();
}
  
//--------------------------------------------------------------------
// Set service parameters.
void util::FileCatalogMetadataExtras::reconfigure(fhicl::ParameterSet const& pset)
{
  std::vector<std::string> md = pset.get<std::vector<std::string> >("Metadata");
  fGeneratePerFileMetadata = pset.get<bool>("GeneratePerFileMetadata");
  fCopyMetadataAttributes = pset.get<std::vector<std::string> >("CopyMetadataAttributes");
  fRenameTemplate = pset.get<std::string>("RenameTemplate");
  fRenameOverwrite = pset.get<bool>("RenameOverwrite");

  // Process name-value pairs.

  if(md.size() %2 != 0)
    throw cet::exception("FileCatalogMetadataExtras")
      << "Metadata array has odd number of entries.\n";
  for(unsigned int i=0; i<md.size(); i += 2)
    fPerJobMetadata.insert(std::pair<std::string, std::string>(md[i], md[i+1]));

  return;
}

//--------------------------------------------------------------------
// PostBeginJob callback.
// Insert per-job metadata via FileCatalogMetadata service.
void util::FileCatalogMetadataExtras::postBeginJob()
{
  checkOutputFiles();

  // Get art metadata service.

  art::ServiceHandle<art::FileCatalogMetadata> mds;

  // Loop over metadata.

  for(auto i=fPerJobMetadata.cbegin(); i!=fPerJobMetadata.cend(); ++i) {
    const std::string& name = i->first;
    const std::string& value = i->second;

    // Ignore null values.

    if(value.size() > 0) {

      // See if this (name, value) already exists.

      bool exists = false;
      art::FileCatalogMetadata::collection_type md;
      mds->getMetadata(md);
      for(auto const & nvp : md) {
	if(nvp.first == name) {
	  exists = true;

	  // If value doesn't match, throw an exception.

	  if(nvp.second != value) {
	    throw cet::exception("FileCatalogMetadataExtras")
	      << "Found duplicate name " << name << " with non-matching value.\n";
	  }
	}
      }

      // Add new (name, value).

      if(!exists)
	mds->addMetadata(name, value);
    }
  }
}

//--------------------------------------------------------------------
// PostEndJob callback.
void util::FileCatalogMetadataExtras::postEndJob()
{
  checkOutputFiles();
}

//--------------------------------------------------------------------
// PostOpenFile callback.
void util::FileCatalogMetadataExtras::postOpenFile(std::string const& fn)
{
  fLastInputFile = fn;
  checkOutputFiles();
}

//--------------------------------------------------------------------
// PostCloseFile callback.
void util::FileCatalogMetadataExtras::postCloseFile()
{
  checkOutputFiles();
}

//--------------------------------------------------------------------
// PreEvent callback.
void util::FileCatalogMetadataExtras::preEvent(art::Event const& /* evt */)
{
  checkOutputFiles();
}

//--------------------------------------------------------------------
// PostEvent callback.
void util::FileCatalogMetadataExtras::postEvent(art::Event const& evt)
{
  checkOutputFiles();

  // Update metadata for open output files.

  art::RunNumber_t run = evt.run();
  art::SubRunNumber_t subrun = evt.subRun();
  art::EventNumber_t event = evt.event();

  for(auto const& fn : fOutputFiles) {
    auto iMap = fPerFileMetadataMap.find(fn);
    if (iMap == fPerFileMetadataMap.end()) {
      throw cet::exception("FileCatalogMetadataExtras")
        << "no metadata for output file '" << fn << "'\n";
    }
    PerFileMetadata& md = iMap->second;

    if(md.fRunNumbers.count(run) == 0)
      md.fRunNumbers.insert(run);
    if(md.fSubRunNumbers.count(subrun) == 0)
      md.fSubRunNumbers.insert(subrun);
    if(md.fEventCount == 0)
      md.fFirstEvent = event;
    md.fLastEvent = event;
    ++md.fEventCount;
    if(!fLastInputFile.empty() && md.fParents.count(fLastInputFile) == 0)
      md.fParents.insert(fLastInputFile);
  }
}

//--------------------------------------------------------------------
// PostOpenOutputFile callback.
void util::FileCatalogMetadataExtras::postOpenOutputFile(std::string const& fn)
{
  // Add initial per-file metadata for this output file.

  if(fPerFileMetadataMap.count(fn) != 0)
    throw cet::exception("FileCatalogMetadataExtras")
      << "Output file " << fn << " already has metadata.\n";
  PerFileMetadata md;
  md.fStartTime = time(0);
  md.fEndTime = md.fStartTime;

  // Extract data from current input file.

  if(fLastInputFile.size() != 0) {
    md.fParents.insert(fLastInputFile);

    if(isArtFile(fLastInputFile) && fCopyMetadataAttributes.size() != 0) {

      // Open sqlite database from input file.

      TFile* file = TFile::Open(fLastInputFile.c_str(), "READ");
      if(file != 0 && !file->IsZombie() && file->IsOpen()) {

	// Open the sqlite datatabase.

	art::SQLite3Wrapper sqliteDB(file, "RootFileDB");
	if(sqliteDB) {

	  // Construct query to read all sam metadata.

	  sqlite3_stmt *stmt = 0;
	  int ok = sqlite3_prepare_v2(sqliteDB, "SELECT Name, Value FROM FileCatalog_metadata;",
				      -1, &stmt, NULL);

	  // Above statement will return an error if this art file doesn't
	  // contain sam metadata.

	  if(ok == SQLITE_OK) {

	    // Loop over all rows returned by query.

	    while((ok = sqlite3_step(stmt)) == SQLITE_ROW) {

	      // Reinterpret cast below is necessary because return
	      // type from sqlite3_colum_text is const unsigned char*,
	      // which has no automatic conversion to const char*.

	      std::string name =
		std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
	      std::string value =
		std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));

	      // Loop over copy attributes.  Add any matching names to
	      // general per-file metadata.

	      for(auto const& attr : fCopyMetadataAttributes) {
		if(name == attr)
		  md.fNVPairs.insert(std::pair<std::string, std::string>(name, value));
	      }
	    }
	    sqlite3_finalize(stmt);
	  }
	}
      }
    }
  }

  fPerFileMetadataMap[fn] = md;
}

//--------------------------------------------------------------------
// PostCloseOutputFile callback.
void util::FileCatalogMetadataExtras::postCloseOutputFile(art::OutputFileInfo const& finfo)
{
  const std::string& fn = finfo.fileName();

  // Update metadata for this output file.

  addPerFileMetadata(fn);

  // Rename output file.

  renameOutputFile(fn);
}

//--------------------------------------------------------------------
// Check whether the specified file is a readable art file.
// Do this by opening the file and checking whether it contains 
// a RootFileDB object.
bool util::FileCatalogMetadataExtras::isArtFile(std::string const& fn)
{
  bool result = false;
  if(fn.size() != 0) {

    // Try to open the file for reading.
    // Apparently, TFile will sometimes throw an exception.

    TFile* file = 0;
    try {
      file = TFile::Open(fn.c_str(), "READ");
    }
    catch (...) {
      file = 0;
    }

    if(file != 0 && !file->IsZombie() && file->IsOpen()) {

      // File successfully opened.
      // Check whether the file contains a RootFileDB object.

      TKey* key = file->GetKey("RootFileDB");
      if(key != 0)
	result = true;
    }

    // Close file.

    if(file != 0) {
      if(file->IsOpen())
	file->Close();
      delete file;
    }
  }

  // Done.

  return result;
}

//--------------------------------------------------------------------
// Check output files.
void util::FileCatalogMetadataExtras::checkOutputFiles()
{
  // In this method we generate pseudo-callbacks for opening and
  // closing output files.  We have to do it this way, because the art
  // ActivityRegistry doesn't currently support callbacks for output
  // files.

  // We can skip all of this stuff if we have not been asked to
  // generate per-file metadata.

  if(!fGeneratePerFileMetadata)
    return;

  // Get a sorted list of currently open output files.

  std::vector<std::string> output_files;
  TIter next(gROOT->GetListOfFiles());
  while(TFile* file = (TFile*)next()) {
    if(file->GetBytesWritten() > 0)
      output_files.push_back(file->GetName());
  }
  std::sort(output_files.begin(), output_files.end());

  // Compare current output files with previously known output files.

  std::vector<std::string> opened_files(output_files.size());
  std::vector<std::string> closed_files(fOutputFiles.size());
  std::vector<std::string>::iterator it;
  it = std::set_difference(output_files.begin(), output_files.end(),
			   fOutputFiles.begin(), fOutputFiles.end(),
			   opened_files.begin());
  opened_files.resize(it - opened_files.begin());
  it = std::set_difference(fOutputFiles.begin(), fOutputFiles.end(),
			   output_files.begin(), output_files.end(),
			   closed_files.begin());
  closed_files.resize(it - closed_files.begin());

  // Generate pseudo-callbacks for opened and closed output files.
  // As of art v1_08_00, callbacks for closed output files are provided
  // by art ActivityRegistry.

  for(auto const& of : opened_files)
    postOpenOutputFile(of);
  
  // Update list of open output files.

  fOutputFiles.swap(output_files);
}

//--------------------------------------------------------------------
// Check output files.
void util::FileCatalogMetadataExtras::addPerFileMetadata(std::string const& fn)
{
  // Do nothing if generating per-file metadata is disabled.

  if(!fGeneratePerFileMetadata)
    return;

  // Do nothing if this is not an art file (not an error).

  if(!isArtFile(fn))
    return;

  // Locate metadata.
  // If we don't have metadata for this file in the metadata map,
  // look for a renamed (non-existing) file.

  std::string map_fn = fn;
  if(fPerFileMetadataMap.count(map_fn) == 0) {
    std::vector<std::string> renamed_files;
    for(auto const& map_ele : fPerFileMetadataMap) {
      std::string filename = map_ele.first;
      std::ifstream file(filename);
      if(file.good() && file.is_open())
	file.close();
      else
	renamed_files.push_back(filename);
    }

    if(renamed_files.size() == 1)
      map_fn = renamed_files.front();
    else {
      throw cet::exception("FileCatalogMetadataExtras")
	<< "Could not access metadata because there is more than one renamed output file.\n";
    }
  }
  if(map_fn != fn) {
    mf::LogInfo info("FileCatalogMetadataExtras");
    info << "No metadata for file " << fn 
	 << "\nUsing renamed file " << map_fn << " metadata instead.";
  }

  if(fPerFileMetadataMap.count(map_fn) == 0)
    throw cet::exception("FileCatalogMetadataExtras")
      << "No metadata found for file " << map_fn << ".\n";
  PerFileMetadata& md = fPerFileMetadataMap[map_fn];

  // Update end time.

  md.fEndTime = time(0);

  // Update sam metadata in root file.  
  // Open exsiting root file for update.

  TFile* file = TFile::Open(fn.c_str(), "UPDATE");
  if(file != 0 && !file->IsZombie() && file->IsOpen()) {

    // Open the sqlite datatabase.

    art::SQLite3Wrapper sqliteDB(file, "RootFileDB");
    if(sqliteDB) {

      // Sqlite database inside art file successfully opened.
      //
      // Before attempting to add per-file sam metadata, test
      // whether the FileCatalog_metadata table exists.  It is
      // normal for the FileCatalog_metadata table to not exist,
      // since generating sam metadata is optional.

      art::SQLErrMsg errMsg;
      sqlite3_exec(sqliteDB, "BEGIN TRANSACTION;", 0, 0, errMsg);
      sqlite3_stmt *stmt = 0;
      int ok = sqlite3_prepare_v2(sqliteDB, "SELECT 1 FROM FileCatalog_metadata;",
				  -1, &stmt, NULL);
      if(ok == SQLITE_OK) {
	sqlite3_step(stmt);
	ok = sqlite3_finalize(stmt);
      }
      if(ok == SQLITE_OK) {

	// Now we have verified that the sqlite database in this art
	// file contqains a FileCatalog_metadata table.

	// Convert our per-file metadata to name-value pairs.

	std::multimap<std::string, std::string> mdmap;
	md.fillMetadata(mdmap);

	// Insert metadata into sqlite database.
	// This loop is basically copied exactly from RootOutputFile.cc.

	sqlite3_prepare_v2(sqliteDB,
			   "INSERT INTO FileCatalog_metadata(Name, Value) VALUES(?, ?);",
			   -1, &stmt, NULL);
	sqlite3_stmt *delete_stmt = 0;
	sqlite3_prepare_v2(sqliteDB,
			   "DELETE FROM FileCatalog_metadata WHERE Name=?;",
			   -1, &delete_stmt, NULL);
	std::string lastName;
	for ( auto const & nvp : mdmap ) {
	  std::string const & theName  (nvp.first);
	  std::string const & theValue (nvp.second);

	  // On the first occurrence of each per-file metadata name,
	  // delete any existing (per-job) metadata with the same
	  // name.

	  if(theName != lastName) {
	    lastName = theName;
	    if(theName.size() != 0) {
	      sqlite3_bind_text(delete_stmt, 1, theName.c_str(),
				theName.size() + 1, SQLITE_STATIC);
	      sqlite3_step(delete_stmt);
	      sqlite3_reset(delete_stmt);
	      sqlite3_clear_bindings(delete_stmt);
	    }
	  }
	  sqlite3_bind_text(stmt, 1, theName.c_str(),
			    theName.size() + 1, SQLITE_STATIC);
	  sqlite3_bind_text(stmt, 2, theValue.c_str(),
			    theValue.size() + 1, SQLITE_STATIC);
	  sqlite3_step(stmt);
	  sqlite3_reset(stmt);
	  sqlite3_clear_bindings(stmt);
	}
	sqlite3_finalize(stmt);
	sqlite3_finalize(delete_stmt);
	sqlite3_exec(sqliteDB, "END TRANSACTION;", 0, 0, errMsg);
      }
      else {

	// The else clause is reached if there is no
	// FileCatalog_metadata table.

	sqlite3_exec(sqliteDB, "ROLLBACK TRANSACTION;", 0, 0, errMsg);
      }
      errMsg.throwIfError();
    }
  }

  // Close (possibly updated) root file.

  if(file != 0) {
    if(file->IsOpen())
      file->Close();
    delete file;
  }

  // Delete the metadata we used from the metadata map, so we don't 
  // accidentally use it again.

  fPerFileMetadataMap.erase(map_fn);
}

//--------------------------------------------------------------------
// Convert per-file metadata to name-value pairs.
void util::FileCatalogMetadataExtras::PerFileMetadata::
fillMetadata(std::multimap<std::string, std::string>& md)
{
  for(auto run : fRunNumbers) {
    std::ostringstream ostr;
    ostr << run;
    md.insert(std::pair<std::string, std::string>("run", ostr.str()));
  }
  for(auto subrun : fSubRunNumbers) {
    std::ostringstream ostr;
    ostr << subrun;
    md.insert(std::pair<std::string, std::string>("subRun", ostr.str()));
  }
  {
    std::ostringstream ostr;
    ostr << fFirstEvent;
    md.insert(std::pair<std::string, std::string>("firstEvent", ostr.str()));
  }
  {
    std::ostringstream ostr;
    ostr << fLastEvent;
    md.insert(std::pair<std::string, std::string>("lastEvent", ostr.str()));
  }
  {
    std::ostringstream ostr;
    ostr << fEventCount;
    md.insert(std::pair<std::string, std::string>("eventCount", ostr.str()));
  }
  {
    std::ostringstream ostr;
    ostr << fStartTime;
    md.insert(std::pair<std::string, std::string>("startTime", ostr.str()));
  }
  {
    std::ostringstream ostr;
    ostr << fEndTime;
    md.insert(std::pair<std::string, std::string>("endTime", ostr.str()));
  }
  for(auto parent : fParents) {
    size_t n = parent.find_last_of('/');
    size_t f = (n == std::string::npos ? 0 : n+1);
    md.insert(std::pair<std::string, std::string>("parent", parent.substr(f)));
  }
  for(auto const& nvp : fNVPairs)
    md.insert(nvp);
}

//--------------------------------------------------------------------
// Rename the specified file according to template specified via
// fcl parameter fRenameTemplate.
void util::FileCatalogMetadataExtras::renameOutputFile(std::string const& fn)
{
  // If the rename template is an empty string, do nothing.

  if(fRenameTemplate.size() == 0)
    return;

  // Make sure the original file is a readable art file.
  // Do nothing if it is not (not an error).

  if(isArtFile(fn)) {

    // Expand the output template.

    std::string new_fn = expandTemplate();
    if(new_fn.size() != 0) {

      // Test whether a file with the new name already exists.
      // If file does exist, action depends on value of fRenameOverwrite
      // parameter.

      bool do_rename = false;
      std::ifstream file(new_fn);
      if(file.good()) {
	if(fRenameOverwrite) {

	  // File exists, but overwriting is enabled.
	  // Delete the existing file and proceed with renaming.

	  remove(new_fn.c_str());
	  do_rename = true;
	}
	else {

	  // File exists, and overwriting is not enabled.
	  // Print a warning and do not rename.

	  mf::LogWarning("FileCatalogMetadataExtras")
	    << "Rename failed because a file already exists with name " << new_fn << std::endl;
	  do_rename = false;
	}
      }
      else {

	// Target file does not exist.  Proceed with rename.

	do_rename = true;
      }

      // Do the rename.

      if(do_rename) {
	mf::LogInfo("FileCatalogMetadataExtras")
	  << "Renaming " << fn << " to " << new_fn << std::endl;
	rename(fn.c_str(), new_fn.c_str());
      }
    }

    // Increment output file count (only count art files).

    ++fOutputFileCount;
  }
}

//--------------------------------------------------------------------
// Convert the output file name template to an actual file name.
std::string util::FileCatalogMetadataExtras::expandTemplate() const
{
  // Make a copy of the template that we can modify, which will be
  // the eventual return value.

  std::string filename = fRenameTemplate;

  // If template doesn't contain any ${...} expressions, stick the
  // field "${bnum 0} before the file extension.

  if(filename.find_first_of("${}") == std::string::npos) {
    size_t n = filename.find_last_of('.');
    if(n == std::string::npos)
      n = filename.size();
    std::string head = filename.substr(0, n);
    std::string tail = filename.substr(n);
    filename = head + std::string("${bnum 0}") + tail;
  }

  // Get the current system time (for ${date} ane ${time}).

  time_t curtime = time(0);

  // Parse file name, looking for ${...} expressions.  We assume these
  // expressions are not nested, and can be evaluated in any order.
  // The text inside the braces, can be either a) a keyword plus
  // arguments, or b) an environment variable.  If neither is the case,
  // a warning is printed and the value of the subargument, if any,
  // is substituted.

  size_t f = 0;
  while((f = filename.find("${")) != std::string::npos) {

    // Find closing brace.  Throw exception in case of problem.

    size_t n = filename.substr(f).find("}");
    if(n == std::string::npos)
      throw cet::exception("FileCatalogMetadataExtras")
	<< "Output file name template: " << filename
	<< " has mismatched braces.\n";

    // Split current filename into three pieces: head, argument of current 
    // ${...}, and tail.

    std::string head = filename.substr(0, f);
    std::string arg = filename.substr(f+2, n-2);
    std::string tail = filename.substr(f+n+1);

    // Make sure that the head and argument string don't contain
    // any of the reserved characters ${}.  This check catches
    // nested ${...} expressions, mismatched braces, and
    // other typos.

    if(head.find_first_of("${}") != std::string::npos ||
       arg.find_first_of("${}") != std::string::npos)
      throw cet::exception("FileCatalogMetadataExtras")
	<< "Problem parsing output file name template: " << filename << ".\n";

    // Extract the first and second word out of the argument string,
    // which we interpret as keyword and subargument.

    std::string keyword;
    std::string subarg;
    {
      std::istringstream istr(arg);
      istr >> keyword;
      istr >> subarg;
    }

    // Do interpretation of keyword and subargument.

    std::string expanded;
    if(keyword == "base") {

      // Base name of input file.

      size_t n = fLastInputFile.find_last_of('/');
      size_t f = (n == std::string::npos ? 0 : n+1);
      expanded = fLastInputFile.substr(f);
      if(subarg.size() != 0 && expanded.rfind(subarg) == expanded.size() - subarg.size())
	expanded = expanded.substr(0, expanded.size() - subarg.size());
    }
    else if(keyword == "dir") {

      // Directory part of path.
      // If path doesn't include a directory, expand to ".".

      size_t n = fLastInputFile.find_last_of('/');
      if(n != std::string::npos)
	expanded = fLastInputFile.substr(0, n);
      else
	expanded = ".";
    }
    else if(keyword == "path") {

      // Full input file path.

      expanded = fLastInputFile;
      if(subarg.size() != 0 && expanded.rfind(subarg) == expanded.size() - subarg.size())
	expanded = expanded.substr(0, expanded.size() - subarg.size());
    }
    else if(keyword == "num") {

      // Output file count.
      // Note that outputFileCount_ is incremented after this method is called,
      // so outputFileCount_ is zero on the first call.
      // Use the subargument to define an offset.

      unsigned int offset = 1;
      if(subarg.size() != 0) {
	std::istringstream istr(subarg);
	istr >> offset;
      }
      std::ostringstream ostr;
      ostr << fOutputFileCount + offset;
      expanded = ostr.str();
    }
    else if(keyword == "bnum") {

      // Output file count.
      // Same as "num," except expands to empty string for first file.

      unsigned int offset = 1;
      if(subarg.size() != 0) {
	std::istringstream istr(subarg);
	istr >> offset;
      }
      if(fOutputFileCount > 0) {
	std::ostringstream ostr;
	ostr << fOutputFileCount + offset;
	expanded = ostr.str();
      }
    }
    else if(keyword == "date") {

      // Date formatted as YYYYMMDD in local time zone.

      struct tm ts;
      struct tm* pts = 0;
      pts = localtime_r(&curtime, &ts);
      if(pts) {
	std::ostringstream ostr;
	ostr << std::setw(4) << std::setfill('0') << pts->tm_year + 1900
	     << std::setw(2) << pts->tm_mon + 1
	     << std::setw(2) << pts->tm_mday;
	expanded = ostr.str();
      }
      else {

	// localtime call failed.

	expanded = "00000000";
      }
    }
    else if(keyword == "time") {

      // Time of day formatted as HHMMSS in local time zone.

      struct tm ts;
      struct tm* pts = 0;
      pts = localtime_r(&curtime, &ts);
      if(pts) {
	std::ostringstream ostr;
	ostr << std::setw(2) << std::setfill('0') << pts->tm_hour
	     << std::setw(2) << pts->tm_min
	     << std::setw(2) << pts->tm_sec;
	expanded = ostr.str();
      }
      else {

	// localtime call failed.

	expanded = "000000";
      }
    }
    else {

      // If we get here, the keyword is not known.
      // Try to interpret the keyword as an environment variable.

      const char* p = getenv(keyword.c_str());
      if(p != 0 && *p != 0)
	expanded = p;
      else {

	// Environment variable not defined.

	mf::LogWarning("FileCatalogMetadataExtras")
	  << "Unknown keyword " << keyword 
	  << " in output file name template " << filename << ".\n";
	expanded = subarg;
      }
    }

    // Make sure no reserved characters made it into the expanded argument.
    // This ensures we can't get into an infinite loop.

    if(expanded.find_first_of("${}") != std::string::npos)
      throw cet::exception("FileCatalogMetadataExtras")
	<< "Problem parsing output file name template: " << filename << ".\n";

    // Reassemble file name.

    filename = head + expanded + tail;
  }

  // Make sure that the finished file name doesn't contain
  // reserved characters ${}.

  if(filename.find_first_of("${}") != std::string::npos)
    throw cet::exception("FileCatalogMetadataExtras")
      << "Problem parsing output file name template: "<< filename << ".\n";
  return filename;
}

namespace util{
 
  DEFINE_ART_SERVICE(FileCatalogMetadataExtras)

} // namespace util
