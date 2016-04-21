/**
 * @file   SpacePointDumpers.h
 * @brief  Functions dumping space points
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 18th, 2015
 * @see    SpacePointDumpers.cc DumpSpacePoints_module.cc
 */

#ifndef LARDATA_RECOBASE_DUMPERS_SPACEPOINTDUMPERS_H
#define LARDATA_RECOBASE_DUMPERS_SPACEPOINTDUMPERS_H 1


// LArSoft libraries
#include "lardata/RecoBase/SpacePoint.h"

// C/C++ standard libraries
#include <string>
#include <utility> // std::forward<>()
#include <type_traits> // std::decay<>


// --- for the implementation ---
// LArSoft libraries
#include "lardata/RecoBaseArt/Dumpers/NewLine.h"


namespace recob {
  namespace dumper {
    
    /// Dumps the content of the specified space point (indentation info in nl)
    /// @tparam Stream the type of the output stream
    /// @tparam NewLineRef NewLine reference type (to get a universal reference)
    template <typename Stream, typename NewLineRef>
    std::enable_if_t
      <std::is_same<NewLine<std::decay_t<Stream>>, std::decay_t<NewLineRef>>::value>
    DumpSpacePoint
      (Stream&& out, recob::SpacePoint const& sp, NewLineRef&& nl);
    
    
    /**
     * @brief Dumps the content of the specified space point into a stream
     * @tparam Stream the type of output stream
     * @param out the output stream
     * @param sp the space point to be dumped
     * @param indent indentation string (none by default)
     * @param indentFirst whether to indent the first line (yes by default)
     * 
     * Insertion operators are required that insert into Stream basic types.
     * 
     * This function does not insert a end-of-line after its output.
     */
    template <typename Stream>
    void DumpSpacePoint(Stream&& out, recob::SpacePoint const& sp,
      std::string indent = "",
      bool indentFirst = true
      )
      {
        DumpSpacePoint(
          std::forward<Stream>(out), sp, makeNewLine(out, indent, !indentFirst)
          );
      }
    
    
  } // namespace dumper
} // namespace recob


//==============================================================================
//=== template implementation
//===
//------------------------------------------------------------------------------
//--- recob::dumper::DumpSpacePoint
//---
template <typename Stream, typename NewLineRef>
std::enable_if_t
  <std::is_same<recob::dumper::NewLine<std::decay_t<Stream>>, std::decay_t<NewLineRef>>::value>
recob::dumper::DumpSpacePoint
  (Stream&& out, recob::SpacePoint const& sp, NewLineRef&& nl)
{
  
  double const* pos = sp.XYZ();
  double const* err = sp.ErrXYZ();
  
  nl()
    << "ID=" << sp.ID() << " at (" << pos[0] << ", " << pos[1] << ", " << pos[2]
    << ") cm, chi^2/NDF=" << sp.Chisq();
  
  nl()
    << "variances { x^2=" << err[0] << " y^2=" << err[2] << " z^2=" << err[5]
    << " xy=" << err[1] << " xz=" << err[3] << " yz=" << err[4] << " }";
  
} // recob::dumper::DumpSpacePoint()

//------------------------------------------------------------------------------

#endif // LARDATA_RECOBASE_DUMPERS_SPACEPOINTDUMPERS_H

