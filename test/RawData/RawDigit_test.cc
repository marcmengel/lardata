/**
 * @file    RawDigit_test.cc
 * @brief   Simple test on a raw::RawDigit object
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    20150114
 * @version 1.0
 *
 * This test simply creates raw::RawDigit objects and verifies that the values
 * it can access are the right ones.
 *
 * See http://www.boost.org/libs/test for the Boost test library home page.
 * 
 * Timing:
 * version 1.0: <1" (debug mode)
 */

// C/C++ standard library
#include <vector>
#include <algorithm> // std::equal()


// Boost libraries
/*
 * Boost Magic: define the name of the module;
 * and do that before the inclusion of Boost unit test headers
 * because it will change what they provide.
 * Among the those, there is a main() function and some wrapping catching
 * unhandled exceptions and considering them test failures, and probably more.
 * This also makes fairly complicate to receive parameters from the command line
 * (for example, a random seed).
 */
#define BOOST_TEST_MODULE ( rawdigit_test )
#include "boost/test/auto_unit_test.hpp" // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK()

// LArSoft libraries
#include "SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "SimpleTypesAndConstants/geo_types.h" // geo::View_t
#include "RawData/raw.h"
#include "RawData/RawDigit.h"



//------------------------------------------------------------------------------
//--- Test code
//


void CheckRawDigit(
  raw::RawDigit const& digits,
  raw::ChannelID_t channel,
  unsigned short samples,
  raw::RawDigit::ADCvector_t const& uncompressed_adclist,
  raw::Compress_t compression
) {
  
  // this is a parameters validation check
  BOOST_CHECK_EQUAL(samples, uncompressed_adclist.size());
  
  // verify that the values are as expected
  // - channel ID
  BOOST_CHECK_EQUAL(digits.Channel(), channel);
  
  // - compression mode
  BOOST_CHECK_EQUAL(digits.Compression(), compression);
  
  // - uncompressed size
  BOOST_CHECK_EQUAL(digits.Samples(), samples);
  BOOST_CHECK_EQUAL(digits.Samples(), uncompressed_adclist.size());
  
  // - digits
  
  // decompress (on an already allocated buffer)
  raw::RawDigit::ADCvector_t ADCs(digits.Samples());
  raw::Uncompress(digits.ADCs(), ADCs, digits.Compression());
  
  BOOST_WARN(digits.NADC() <= samples); // is this always the case?
  BOOST_CHECK
    (std::equal(ADCs.begin(), ADCs.end(), uncompressed_adclist.begin()));
  
  // - others
  BOOST_CHECK_EQUAL(digits.GetPedestal(), 0.);
  BOOST_CHECK_EQUAL(digits.GetSigma(), 0.);
  
} // CheckRawDigit()


void RawDigitTestDefaultConstructor() {
  
  //
  // Part I: initialization of wire inputs
  //
  // these are the values expected for a default-constructed wire
  const raw::ChannelID_t channel = raw::InvalidChannelID;
  const unsigned short samples = 0;
  raw::RawDigit::ADCvector_t adclist;
  const raw::Compress_t compression = raw::kNone;
// raw::RawDigit::Flags_t flags;
  
  //
  // Part II: default constructor
  //
  // step II.1: create a wire with the signal-copying constructor
  raw::RawDigit digits;
  
  
  // step II.2: verify that the values are as expected
  CheckRawDigit(digits, channel, samples, adclist, compression);
  
} // RawDigitTestDefaultConstructor()


void RawDigitTestCustomConstructors() {
  
  //
  // Part I: initialization of wire inputs
  //
  const raw::ChannelID_t channel = 12;
  const unsigned short samples = 1000;
  raw::RawDigit::ADCvector_t adclist(samples);
  for (size_t i = 0; i < samples; ++i)
    adclist[i] = (i % 3)? 0: i;
  const raw::Compress_t compression = raw::kHuffman;
// raw::RawDigit::Flags_t flags;
  
  // working a copy of the original data:
  std::vector<short> buffer(adclist);
  raw::Compress(buffer, compression); // compression happens in place
  
  
  //
  // Part II: constructor with signal copy
  //
  // step II.1: create a wire with the signal-copying constructor
  raw::RawDigit digits1(channel, samples, buffer, compression /*, flags */);
  
  // step II.2: verify that the values are as expected
  CheckRawDigit(digits1, channel, samples, adclist, compression);
  
  
  //
  // Part II: constructor with signal copy
  //
  // step II.1: create a wire with the signal-copying constructor
  std::vector<short> buffer_copy(buffer);
  raw::RawDigit digits2
    (channel, samples, std::move(buffer_copy), compression /*, flags */);
  
  // step II.2: verify that the values are as expected
  CheckRawDigit(digits2, channel, samples, adclist, compression);
  
} // RawDigitTestCustomConstructors()


//------------------------------------------------------------------------------
//--- registration of tests
//
// Boost needs now to know which tests we want to run.
// Tests are "automatically" registered, hence the BOOST_AUTO_TEST_CASE()
// macro name. The argument is the name of the test; each step may have a
// number of checks and it will fail if any of them does.
//

BOOST_AUTO_TEST_CASE(RawDigitDefaultConstructor) {
  RawDigitTestDefaultConstructor();
}

BOOST_AUTO_TEST_CASE(RawDigitCustomConstructors) {
  RawDigitTestCustomConstructors();
}
