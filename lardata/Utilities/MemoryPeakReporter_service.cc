/**
 * @file   MemoryPeakReporter_service.cc
 * @brief  Service reporting the memory peak for the job
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   August 1st, 2014
 * @see    SimpleMemoryCheck_service.cc
 *
 * This service has no public interface (no header file).
 * It is supposed to be fully driven by the art plug in system.
 */


// detect if we are running under Linux
#ifdef __linux__
#	define LINUX 1
#endif

// C/C++ standard library
#include <string> // std::getline(), std::string
#include <sstream>
#include <fstream>
#include <ios> // std::fixed
#include <iomanip> // std::setprecision()

// POSIX libraries
#include <unistd.h> // getpid()

// art etc.
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Persistency/Provenance/ModuleDescription.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"


namespace lar {
  
  /**
   * @brief Service reporting the memory peak for the job
   *
   * The service asks the Linux kernel for the peak memory so far.
   * This is supposed to be the largest Virtual Size (VSIZE) the operating
   * system has ever given to this process so far.
   * 
   * This service is ad interim, until its functionality is absorbed by
   * SimpleMemoryCheck service.
   * 
   * Reports are sent into mf::LogInfo stream.
   * 
   * <b>Configuration parameters</b>
   * - <b>OnEventIncrease</b> (boolean, default: true) reports an increase at
   *   the end of each event
   * - <b>OnEveryEvent</b> (boolean, default: false) reports the peak at
   *   the end of each event
   * - <b>OnModuleIncrease</b> (boolean, default: true) reports an increase at
   *   the end of each module
   * - <b>OnEveryModule</b> (boolean, default: false) reports the peak at
   *   the end of each module
   * - <b>OutputCategory</b> (string, default: "MemoryPeak"): output category
   *   of mf::LogInfo used for the messages
   *
   * @todo Make it MacOS-compatible (if possible) with the help of MacOS experts
   */
  class MemoryPeakReporter {
      public:
    using MemSize_t = unsigned long long; ///< type for memory sizes
    
    typedef enum {
      rmNever,        ///< no report
      rmOnIncrease, ///< only if increase
      rmAlways,     ///< always
    } ReportMode_t; ///< type and time of report
    
    
    /// Default constructor
    MemoryPeakReporter
      (fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    
    
    /// Reports the peak memory
    void Report
      (std::string type, std::string modLabel = "", std::string modName = "")
      const;
    
    
    /// Updates the peak memory
    /// @return whether the peak has increased
    bool UpdatePeak();
    
    
    /// Executed at the end of each module
    void postModule(const art::ModuleDescription& md)
      {
        if (shouldReport(ModuleReportMode))
          Report("module", md.moduleLabel(), md.moduleName());
      } // postModule()


    /// Executed at the end of each event
    void postEventProcessing(const art::Event&)
      { if (shouldReport(EventReportMode)) Report("event"); }

    
    /// Executed after a source has been created
    void postSource( art::Event const& ) { UpdatePeak(); Report("source"); }

    
    /// Executed at the end of the job
    void postEndJob() { UpdatePeak(); Report("end"); }

    
    /// Read the peak memory from the operating system
    static MemSize_t ReadPeak();
    
    
      private:
    ReportMode_t ModuleReportMode; ///< how peaks are reported after modules
    ReportMode_t EventReportMode; ///< how peaks are reported after events
    std::string OutputCategory; ///< output category for messages in mf::LogInfo
    
    MemSize_t PeakSoFar = 0; ///< largest peak observed so far (bytes)
    
    /// Updates the peak and returns whether configuration prescribes reporting
    bool shouldReport(ReportMode_t mode = rmAlways);
  }; // class MemoryPeakReporter
  
} // namespace lar


DECLARE_ART_SERVICE(lar::MemoryPeakReporter, LEGACY)



//------------------------------------------------------------------------------
//--- implementation

lar::MemoryPeakReporter::MemoryPeakReporter
  (fhicl::ParameterSet const& pset, art::ActivityRegistry& reg)
  : ModuleReportMode(rmOnIncrease)
  , EventReportMode(rmOnIncrease)
  , OutputCategory  (pset.get<std::string>("OutputCategory", "MemoryPeak"))
{
  // mode of report for modules
  ModuleReportMode
    = pset.get<bool>("OnModuleIncrease", true)? rmOnIncrease: rmNever;
  if (pset.get<bool>("OnEveryModule", false)) ModuleReportMode = rmAlways;
  
  // mode of report for events
  EventReportMode
    = pset.get<bool>("OnEventIncrease", true)? rmOnIncrease: rmNever;
  if (pset.get<bool>("OnEveryEvent", false)) EventReportMode = rmAlways;
  
  #ifdef LINUX
  mf::LogDebug(OutputCategory) << "Linux mode.";
  #else // !LINUX
  // not happy with this? tell me how to fix it!
  // (or just add the relevant code in ReadPeak())
  mf::LogError(OutputCategory)
    << "Sorry, you are out of luck: MemoryPeakReporter is Linux only.";
  #endif
  
  // always report at the end of the job
  reg.sPostEndJob.watch(this, &MemoryPeakReporter::postEndJob);
  
  // maybe report at the end of the event (and of source too)
  if (EventReportMode != rmNever) {
    reg.sPostSourceEvent.watch(this, &MemoryPeakReporter::postSource);
    reg.sPostProcessEvent.watch(this, &MemoryPeakReporter::postEventProcessing);
  }
  
  // maybe report at the end of each event
  if (ModuleReportMode != rmNever) {
    reg.sPostModule.watch(this, &MemoryPeakReporter::postModule);
  }
  
  // let's start: always report at the creation of this service
  UpdatePeak();
  Report("startup");
  
} // lar::MemoryPeakReporter::MemoryPeakReporter()


bool lar::MemoryPeakReporter::UpdatePeak() {
  MemSize_t NewPeak = ReadPeak();
  if (NewPeak <= PeakSoFar) return false;
  PeakSoFar = NewPeak;
  return true;
} // lar::MemoryPeakReporter::UpdatePeak()


void lar::MemoryPeakReporter::Report(
  std::string type,
  std::string modLabel /* = "" */, std::string modName /* = "" */
) const {
  mf::LogInfo info(OutputCategory);
  info << "MemoryPeak: " << type;
  if (!modLabel.empty() || !modName.empty())
    info << " " << modLabel << ":" << modName;
  info << " VMPEAK " << std::fixed << std::setprecision(1)
    << (PeakSoFar / 1048576.) << " MiB";
} // lar::MemoryPeakReporter::Report()


bool lar::MemoryPeakReporter::shouldReport(ReportMode_t mode /* = rmAlways */) {
  bool bIncreased = UpdatePeak();
  return ((mode == rmAlways) || ((mode == rmOnIncrease) && bIncreased));
} // lar::MemoryPeakReporter::shouldReport()


lar::MemoryPeakReporter::MemSize_t lar::MemoryPeakReporter::ReadPeak() {
#ifdef LINUX
  // find the current process ID, then read "/proc/PID/status" from procfs
  std::ostringstream sstr;
  sstr << "/proc/" << getpid() << "/status";
  std::ifstream StatusFile(sstr.str());
  if (!StatusFile) {
    throw art::Exception(art::errors::Configuration)
      << "MemoryPeakReporter: failed to open " << sstr.str();
  }
  
  // parse line by line, looking for relevant lines
  MemSize_t mem = 0;
  std::string line, token;
  std::istringstream linebuf;
  while (std::getline(StatusFile, line)) {
    linebuf.str(line);
    linebuf >> token;
    if (!linebuf) continue;
    if (token == "VmSize:") {
      if (!(linebuf >> mem)) continue;
      linebuf >> token;
      if (!linebuf) continue;
      if (token == "kB") mem *= 1024;
      else if (token == "MB") mem *= 1024*1024;
      else if (token == "GB") mem *= 1024*1024*1024;
      else {
        mem = 0;
        continue;
      }
      break; // nothing more that I want
    } // if VmSize
    else continue;
  } // while
  
  // if mem is still 0, there has been a problem
  if (mem == 0) {
    throw art::Exception(art::errors::InvalidNumber)
      << "can't read VmPeak from " << sstr.str();
  }
  return mem;
#else
  // nothing else implemented yet
  return 0;
#endif
} // lar::MemoryPeakReporter::UpdatePeak()

DEFINE_ART_SERVICE(lar::MemoryPeakReporter)
