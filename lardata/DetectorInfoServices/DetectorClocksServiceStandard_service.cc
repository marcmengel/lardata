#include "lardata/DetectorInfoServices/DetectorClocksServiceStandard.h"
// vim: set sw=2 expandtab :

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Persistency/Provenance/ScheduleContext.h"
#include "art_root_io/RootDB/SQLite3Wrapper.h"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "canvas/Persistency/Provenance/ParameterSetMap.h"
#include "canvas/Persistency/Provenance/rootNames.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandard.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandardTriggerLoader.h"

#include "TFile.h"
#include "TTree.h"

#include "art_root_io/Inputfwd.h"
#include "art_root_io/detail/readMetadata.h"

#include <string>
#include <vector>

using namespace std;

namespace detinfo {

  DetectorClocksServiceStandard::DetectorClocksServiceStandard(fhicl::ParameterSet const& pset,
                                                               art::ActivityRegistry& reg)
    : fClocks{pset}
  {

    reg.sPreProcessEvent.watch(this, &DetectorClocksServiceStandard::preProcessEvent);
    reg.sPostOpenFile.watch(this, &DetectorClocksServiceStandard::postOpenFile);
    reg.sPreBeginRun.watch(this, &DetectorClocksServiceStandard::preBeginRun);
  }

  void
  DetectorClocksServiceStandard::reconfigure(fhicl::ParameterSet const& pset)
  {
    fClocks.Configure(pset);
  }

  void
  DetectorClocksServiceStandard::preProcessEvent(const art::Event& evt, art::ScheduleContext)
  {
    setDetectorClocksStandardTrigger(fClocks, evt);
    setDetectorClocksStandardG4RefTimeCorrection(fClocks, evt);
  }

  void
  DetectorClocksServiceStandard::preBeginRun(art::Run const& run)
  {
    fClocks.ApplyParams();
  }

  void
  DetectorClocksServiceStandard::postOpenFile(const string& filename)
  {
    if (!fClocks.InheritClockConfig()) { return; }
    if (filename.empty()) { return; }
    TFile* file = TFile::Open(filename.c_str(), "READ");
    if ((file == nullptr) || file->IsZombie() || !file->IsOpen()) { return; }
    auto metaDataTree = static_cast<TTree*>(file->Get(art::rootNames::metaDataTreeName().c_str()));
    if (metaDataTree == nullptr) {
      throw cet::exception("DetectorClocksServiceStandard",
                           "Input file does not contain a metadata tree!");
    }
    auto fileFormatVersion = art::detail::readMetadata<art::FileFormatVersion>(metaDataTree);
    vector<string> cfgName(fClocks.ConfigNames());
    vector<double> cfgValue(fClocks.ConfigValues());
    vector<size_t> config_count(kInheritConfigTypeMax, 0);
    vector<double> config_value(kInheritConfigTypeMax, 0);
    if (fileFormatVersion.value_ < 5) {
      art::ParameterSetMap psetMap;
      if (!art::detail::readMetadata(metaDataTree, psetMap)) {
        throw cet::exception("DetectorClocksServiceStandard",
                             "Could not read ParameterSetMap from metadata tree!");
      }
      for (auto const& psEntry : psetMap) {
        fhicl::ParameterSet ps;
        fhicl::make_ParameterSet(psEntry.second.pset_, ps);
        if (!fClocks.IsRightConfig(ps)) { continue; }
        for (size_t i = 0; i < kInheritConfigTypeMax; ++i) {
          double value_from_file = ps.get<double>(cfgName[i]);
          if (config_count[i] == 0) { config_value[i] = value_from_file; }
          else if (config_value[i] != value_from_file) {
            throw cet::exception("DetectorClocksServiceStandard")
              << "Found historical value disagreement for " << cfgName[i] << " ... "
              << config_value[i] << " != " << value_from_file;
          }
          ++config_count[i];
        }
      }
    }
    else {
      art::SQLite3Wrapper sqliteDB(file, "RootFileDB");
      sqlite3_stmt* stmt{nullptr};
      sqlite3_prepare_v2(sqliteDB, "SELECT PSetBlob from ParameterSets;", -1, &stmt, NULL);
      while (sqlite3_step(stmt) == SQLITE_ROW) {
        fhicl::ParameterSet ps;
        fhicl::make_ParameterSet(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), ps);
        if (!fClocks.IsRightConfig(ps)) { continue; }
        for (size_t i = 0; i < kInheritConfigTypeMax; ++i) {
          double value_from_file = ps.get<double>(cfgName[i]);
          if (config_count[i] == 0) { config_value[i] = value_from_file; }
          else if (config_value[i] != value_from_file) {
            throw cet::exception("DetectorClocksServiceStandard")
              << "Found historical value disagreement for " << cfgName[i] << " ... "
              << config_value[i] << " != " << value_from_file;
          }
          ++config_count[i];
        }
      }
    }
    for (size_t i = 0; i < kInheritConfigTypeMax; ++i) {
      if ((config_count[i] != 0) && (cfgValue[i] != config_value[i])) {
        cout << "Overriding configuration parameter " << cfgName[i] << " ... " << cfgValue[i]
             << " (fcl) => " << config_value[i] << " (data file)" << endl;
        fClocks.SetConfigValue(i, config_value[i]);
      }
    }
    if (file != nullptr) {
      if (file->IsOpen()) { file->Close(); }
      delete file;
    }
    fClocks.ApplyParams();
  }

} // namespace detinfo

DEFINE_ART_SERVICE_INTERFACE_IMPL(detinfo::DetectorClocksServiceStandard,
                                  detinfo::DetectorClocksService)
