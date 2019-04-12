////////////////////////////////////////////////////////////////////////
/// \file  LArRawInputDriverShortBo.h
/// \brief Source to convert raw binary files to root files for Short Bo TPC
///
/// \author  brebel@fnal.gov, soderber@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "canvas/Persistency/Provenance/SubRunID.h"

#include <fstream>
#include <string>
#include <vector>

///Conversion of binary data to root files
namespace lris {
  class LArRawInputDriverShortBo;
}

class lris::LArRawInputDriverShortBo {
  /// Class to fill the constraints on a template argument to the class,
  /// FileReaderSource
 public:
  // Required constructor
  LArRawInputDriverShortBo(fhicl::ParameterSet const &pset,
                    art::ProductRegistryHelper &helper,
                    art::SourceHelper const &pm);

  // Required by FileReaderSource:
  void closeCurrentFile();
  void readFile(std::string const &name,
                art::FileBlock* &fb);
  bool readNext(art::RunPrincipal* const &inR,
                art::SubRunPrincipal* const &inSR,
                art::RunPrincipal* &outR,
                art::SubRunPrincipal* &outSR,
                art::EventPrincipal* &outE);

 private:
  // --- data members:
  typedef  std::vector<std::string>  stringvec_t;

  art::SourceHelper const&       principalMaker_;
  std::string                    currentDir_;
  stringvec_t                    inputfiles_;
  stringvec_t::const_iterator    nextfile_;
  stringvec_t::const_iterator    filesdone_;
  art::SubRunID                  currentSubRunID_;
};  // LArRawInputDriverShortBo
