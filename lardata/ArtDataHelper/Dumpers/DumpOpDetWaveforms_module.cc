/**
 * @file   DumpOpDetWaveforms_module.cc
 * @brief  Dumps on screen the content of the raw optical detector waveforms.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 8th, 2017
 */

// LArSoft includes
#include "lardata/Utilities/StatCollector.h" // lar::util::MinMaxCollector
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
#include <algorithm> // std::min()
#include <ios> // std::fixed
#include <iomanip> // std::setprecision(), std::setw()
#include <utility> // std::forward(), std::swap()


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
    
    
    /// Dumps the content of a single waveform into the specified output stream.
    template <typename Stream>
    void DumpWaveform(
      Stream&& out, raw::OpDetWaveform const& waveform,
      std::string indent, std::string indentFirst
      ) const;
    
    template <typename Stream>
    void DumpWaveform(
      Stream&& out, raw::OpDetWaveform const& waveform, std::string indent = ""
      ) const
      { DumpWaveform(std::forward<Stream>(out), waveform, indent, indent); }
    
    
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
    
    mf::LogVerbatim(fOutputCategory)
      << "The event " << event.id() << " contains data for "
      << Waveforms->size() << " optical detector channels";
    if (fPedestal != 0) {
      mf::LogVerbatim(fOutputCategory) << "A pedestal of " << fPedestal
        << " counts will be subtracted from all ADC readings.";
    } // if pedestal
    
    for (raw::OpDetWaveform const& waveform: *Waveforms) {
      
      DumpWaveform(mf::LogVerbatim(fOutputCategory), waveform, "  ");
      
    } // for waveforms
    
  } // DumpOpDetWaveforms::analyze()
  
  
  //----------------------------------------------------------------------------
  template <typename Stream>
  void DumpOpDetWaveforms::DumpWaveform(
    Stream&& out, raw::OpDetWaveform const& waveform,
    std::string indent, std::string indentFirst
    ) const
  {
    auto const& data = waveform;
    using Count_t = raw::ADC_Count_t;
    
    // print a header for the raw digits
    out << indentFirst
      << "on channel #" << waveform.ChannelNumber() << " (time stamp: "
      << waveform.TimeStamp() << "): " << data.size() << " time ticks";
    
    // print the content of the channel
    if (fDigitsPerLine == 0) return;
    
    std::vector<Count_t> DigitBuffer(fDigitsPerLine), LastBuffer;
    
    unsigned int repeat_count = 0; // additional lines like the last one
    unsigned int index = 0;
    
    lar::util::MinMaxCollector<Count_t> Extrema;
    out << "\n" << indent
      << "  content of the channel (" << fDigitsPerLine << " ticks per line):";
    auto iTick = data.cbegin(), tend = data.cend(); // const iterators
    while (iTick != tend) {
      // the next line will show at most fDigitsPerLine ticks
      unsigned int line_size
        = std::min(fDigitsPerLine, (unsigned int) data.size() - index);
      if (line_size == 0) break; // no more ticks
      
      // fill the new buffer (iTick will move forward)
      DigitBuffer.resize(line_size);
      auto iBuf = DigitBuffer.begin(), bend = DigitBuffer.end();
      while ((iBuf != bend) && (iTick != tend))
        Extrema.add(*(iBuf++) = *(iTick++) - fPedestal);
      index += line_size;
      
      // if the new buffer is the same as the old one, just mark it
      if (DigitBuffer == LastBuffer) {
        repeat_count += 1;
        continue;
      }
      
      // if there are previous repeats, write that on screen
      // before the new, different line
      if (repeat_count > 0) {
        out << "\n" << indent
          << "  [ ... repeated " << repeat_count << " more times ]";
        repeat_count = 0;
      }
      
      // dump the new line of ticks
      out << "\n" << indent << " ";
      for (auto digit: DigitBuffer)
        out << " " << std::setw(4) << digit;
      
      // quick way to assign DigitBuffer to LastBuffer
      // (we don't care we lose the former)
      std::swap(LastBuffer, DigitBuffer);
      
    } // while
    if (repeat_count > 0) {
      out << "\n" << indent
        << "  [ ... repeated " << repeat_count << " more times to the end]";
    }
    if (Extrema.min() != Extrema.max()) {
      out << "\n" << indent
        << "  range of " << data.size()
        << " samples: [" << Extrema.min() << ";" << Extrema.max() << "]";
    }
  
  } // DumpOpDetWaveforms::DumpWaveform()
  
  
  //----------------------------------------------------------------------------
  DEFINE_ART_MODULE(DumpOpDetWaveforms)

  //----------------------------------------------------------------------------
  
} // namespace detsim
