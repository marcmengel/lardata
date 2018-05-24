/**
 * @file   lardata/DetectorInfo/DetectorClocksStandardTriggerLoader.h
 * @brief  Functions to load trigger time in `detinfo::DetectorClocksStandard`.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 21, 2018
 * 
 * This header contains framework-dependent functions to:
 * * set `DetectorClocksStandard` trigger time from a `raw::Trigger`
 * * set `DetectorClocksStandard` trigger time from a `raw::Trigger` in an event
 * * do the former, and fall back to default values on failure
 * 
 * These functions are compatible with both _art_ and _gallery_.
 * 
 * This is a pure header library. Effective dependencies include:
 * * `lardata_DetectorInfo`
 * * `lardataobj_RawData`
 * * `canvas`
 * * `cetlib_except`
 * 
 */

#ifndef LARDATA_DETECTORINFO_DETECTORCLOCKSSTANDARDTRIGGERLOADER_H
#define LARDATA_DETECTORINFO_DETECTORCLOCKSSTANDARDTRIGGERLOADER_H

// LArSoft libraries
#include "lardataalg/DetectorInfo/DetectorClocksStandard.h"
#include "lardataobj/RawData/TriggerData.h" // raw::Trigger

// framework libraries
#include "canvas/Utilities/InputTag.h"
#include "cetlib_except/exception.h"

// C++ standard libraries
#include <vector>


namespace detinfo {
  
  /**
   * @brief Loads `DetectorClocksStandard` trigger times from a `raw::Trigger`.
   * @param detClocks the instance of `detinfo::DetectorClocksStandard` to set
   * @param trigger the trigger object to extract timings from
   * 
   * The times are stored unconditionally and directly, with no time scale
   * conversion.
   */
  inline void setDetectorClocksStandardTriggersFromRawTrigger
    (detinfo::DetectorClocksStandard& detClocks, raw::Trigger const& trigger)
    { detClocks.SetTriggerTime(trigger.TriggerTime(), trigger.BeamGateTime()); }


  /**
   * @brief Loads `DetectorClocksStandard` trigger times from a `raw::Trigger`.
   * @tparam Event type of event where trigger data might be stored
   * @param detClocks the instance of `detinfo::DetectorClocksStandard` to set
   * @param event the event the trigger objects are stored into
   * @return whether the times were set
   * @throws cet::exception if trigger data product has more than one trigger
   * 
   * The input tag stored in `detClocks`
   * (`detinfo::DetectorClocksStandard::TrigModuleName()`) is used to read
   * a `raw::Trigger` collection.
   * 
   * If the event has no such trigger data product, or if that data product is
   * empty, `false` is returned and `detClocks` is unchanged.
   * 
   * If the trigger data product is found and there are more than one triggers
   * in there, an exception is thrown: we support only the case where the
   * trigger to be used is unequivocal.
   */
  template <typename Event>
  bool setDetectorClocksStandardTriggersFromEvent
    (detinfo::DetectorClocksStandard& detClocks, Event const& event)
  {
    //
    // fetch the trigger data product
    //
    using TriggerHandle_t
      = typename Event::template HandleT<std::vector<raw::Trigger>>;
    
    art::InputTag const triggerTag = detClocks.TrigModuleName();
    TriggerHandle_t triggerHandle;
    if (!event.template getByLabel(triggerTag, triggerHandle)) return false;
    
    //
    // check that we do have a trigger
    // (we have already checked whether the handle is valid above)
    //
    
    auto const& triggers = *triggerHandle;
    if (triggers.empty()) return false;
    
    //
    // select which trigger to set (i.e., the only one!)
    //
    if (triggers.size() != 1) {
      throw cet::exception("setDetectorClocksStandardTrigger")
        << "Found " << triggers.size()
        << " trigger objects in '" << triggerTag.encode()
        << "' (only one trigger per event is supported)\n";
    }
        
    //
    // set the timings from the first and only trigger into the service
    //
    setDetectorClocksStandardTriggersFromRawTrigger
      (detClocks, triggers.front());
    
    // all done
    return true;
    
  } // setDetectorClocksStandardTriggersFromEvent()
  
  
  /**
   * @brief Loads `DetectorClocksStandard` trigger times.
   * @tparam Event type of event where trigger data might be stored
   * @param detClocks the instance of `detinfo::DetectorClocksStandard` to set
   * @param event the event the trigger objects are stored into
   * @return whether the times were set from a trigger object
   * @throws cet::exception if trigger data product has more than one trigger
   * 
   * This function sets the trigger and beam gate times of `detClocks`.
   * First, it attempts to read the settings from the event (see
   * `setDetectorClocksStandardTriggersFromEvent()`).
   * If that fails "nicely", then sets them with the default values from the
   * configuration.
   * 
   * A "nice" failure is where there is no trigger object in the event.
   * Other types of failure include when there are more than one trigger objects
   * in the event, in which case no choice is made, and an exception is thrown.
   */
  template <typename Event>
  bool setDetectorClocksStandardTrigger
    (detinfo::DetectorClocksStandard& detClocks, Event const& event)
  {
    //
    // try to read the trigger from the event
    //
    if (setDetectorClocksStandardTriggersFromEvent(detClocks, event))
      return true;
    
    //
    // if there was no trigger candidate, set it default
    //
    detClocks.SetDefaultTriggerTime();
    return false;
    
  } // setDetectorClocksStandardTrigger

} // namespace detinfo


#endif // LARDATA_DETECTORINFO_DETECTORCLOCKSSTANDARDTRIGGERLOADER_H
