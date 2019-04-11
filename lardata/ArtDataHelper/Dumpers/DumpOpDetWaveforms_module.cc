/**
 * @file   DumpOpDetWaveforms_module.cc
 * @brief  Dumps on screen the content of the raw optical detector waveforms.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 8th, 2017
 */

// LArSoft includes
#include "lardataalg/Dumpers/RawData/OpDetWaveform.h"
#include "lardataobj/RawData/OpDetWaveform.h"

// art libraries
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Utilities/InputTag.h"

// support libraries
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/Comment.h"

// C//C++ standard libraries
#include <string>


namespace detsim {

  /**
   * @brief Prints the content of all optical detector waveforms on screen.
   *
   * This analyser prints the content of all the raw digits into the
   * LogInfo/LogVerbatim stream.
   * 
   * Configuration parameters
   * -------------------------
   * 
   * - *OpDetWaveformsTag* (string, default: `daq`): input tag of the
   *   raw::OpDetWaveform collection to be dumped
   * - *OutputCategory* (string, default: `DumpOpDetWaveforms`): the category
   *   used for the output (useful for filtering)
   * - *DigitsPerLine* (integer, default: `20`): the dump of ADC readings
   *   will put this many of them for each line
   * - *Pedestal* (integer, default: `0`): ADC readings are written relative
   *   to this number
   *
   */
  class DumpOpDetWaveforms: public art::EDAnalyzer {
      public:
    
    struct Config {
      
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;
      
      fhicl::Atom<art::InputTag> OpDetWaveformsTag {
        Name("OpDetWaveformsTag"),
        Comment("input tag of the raw::OpDetWaveform collection to be dumped")
        };
      
      fhicl::Atom<std::string> OutputCategory {
        Name("OutputCategory"),
        Comment("name of the category used for the output"),
        "DumpOpDetWaveforms"
        };
      
      fhicl::Atom<unsigned int> DigitsPerLine {
        Name("DigitsPerLine"),
        Comment
          ("the dump of ADC readings will put this many of them for each line"),
        20U
        };
      
      fhicl::Atom<raw::ADC_Count_t> Pedestal {
        Name("Pedestal"),
        Comment("ADC readings are written relative to this number"),
        0
        };
      
    }; // struct Config
    
    using Parameters = art::EDAnalyzer::Table<Config>;
    
    
    explicit DumpOpDetWaveforms(Parameters const& config);
    
    
    /// Does the printing.
    void analyze (const art::Event& evt);
    
    
      private:
    
    art::InputTag fOpDetWaveformsTag; ///< Input tag of data product to dump.
    std::string fOutputCategory; ///< Category for `mf::LogInfo` output.
    unsigned int fDigitsPerLine; ///< ADC readings per line in the output.
    raw::ADC_Count_t fPedestal; ///< ADC pedestal (subtracted from readings).
    
  }; // class DumpOpDetWaveforms
  
} // namespace detsim


namespace detsim {

  //-------------------------------------------------
  DumpOpDetWaveforms::DumpOpDetWaveforms(Parameters const& config)
    : EDAnalyzer         (config)
    , fOpDetWaveformsTag (config().OpDetWaveformsTag())
    , fOutputCategory    (config().OutputCategory())
    , fDigitsPerLine     (config().DigitsPerLine())
    , fPedestal          (config().Pedestal())
    {}


  //-------------------------------------------------
  void DumpOpDetWaveforms::analyze(const art::Event& event) {
    
    // fetch the data to be dumped on screen
    auto Waveforms =
      event.getValidHandle<std::vector<raw::OpDetWaveform>>(fOpDetWaveformsTag);
    
    dump::raw::OpDetWaveformDumper dump(fPedestal, fDigitsPerLine);
    dump.setIndent("  ");
    
    mf::LogVerbatim(fOutputCategory)
      << "The event " << event.id() << " contains data for "
      << Waveforms->size() << " optical detector channels";
    if (fPedestal != 0) {
      mf::LogVerbatim(fOutputCategory) << "A pedestal of " << fPedestal
        << " counts will be subtracted from all ADC readings.";
    } // if pedestal
    
    for (raw::OpDetWaveform const& waveform: *Waveforms)
      dump(mf::LogVerbatim(fOutputCategory), waveform);
    
  } // DumpOpDetWaveforms::analyze()
  
  
  //----------------------------------------------------------------------------
  DEFINE_ART_MODULE(DumpOpDetWaveforms)

  //----------------------------------------------------------------------------
  
} // namespace detsim
