/**
 * @file    CountersMap_test.cc
 * @brief   Tests the counter map
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    20140822
 * @version 1.0
 *
 * See http://www.boost.org/libs/test for the Boost test library home page.
 * 
 * Timing:
 * version 1.0 takes less than 3" on a 3 GHz machine FIXME
 */

// C/C++ standard libraries
#include <vector>
#include <map>
#include <random>

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
#define BOOST_TEST_MODULE ( CountersMap_test )
#include <boost/test/auto_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK()

// LArSoft libraries
#include "Utilities/CountersMap.h"


/// The seed for the default random engine
constexpr unsigned int RandomSeed = 12345;


//------------------------------------------------------------------------------
//--- Test code
//

/**
 * @brief Tests with a vector of counter maps (Hough transfort use case)
 *
 * The test consists in filling a lot of points into a 2D sparse "image" (or
 * histogram).
 * Two structures are maintained, one with the standard STL map, another
 * with the CountersMap.
 * The test fails if the two images do not match.
 */
void RunHoughTransformTreeTest() {
  
  // the structure we are testing is a 2D "image" of integers;
  // image is mostly empty (zero), but each abscissa has roughly the same
  // number of non-empty pixels (NPoints), and at least one of them.
  
  constexpr unsigned int NPoints =  1000;
  constexpr unsigned int NAngles = 10800;
  constexpr unsigned int NDist   =  2500; // half distance
  
  typedef std::map<int, int> BaseMap_t;
  
  // STL allocator
  std::vector<std::map<
    int, int, BaseMap_t::key_compare,
    std::allocator<BaseMap_t::value_type>
    >> stl_image(NAngles);
  
  // CountersMap; uses chunks of 8 counters per block
  std::vector<lar::CountersMap<int, int, 8>> cm_image(NAngles);
  // the following should fail compilation
//  std::vector<lar::CountersMap<int, int, 9>> cm_image_broken(NAngles);
  
  static std::default_random_engine random_engine(RandomSeed);
  std::uniform_real_distribution<float> uniform(-1., 1.);
  
  for (unsigned int iPoint = 0; iPoint != NPoints; ++iPoint) {
    // we add here some simple image, not to strain the test;
    // this is a straight line
    const float offset = uniform(random_engine) * NDist;
    const float slope = uniform(random_engine);
    float d = offset;
    for (size_t iAngle = 0; iAngle < NAngles; ++iAngle) {
      // add one entry on the (angle ; distance) plane
      ++(stl_image[iAngle][int(d)]);
      cm_image[iAngle].increment(int(d)); // different interface than usual map
      // prepare for the next point; wrap in the [-NDist, NDist[ range
      d += slope;
      while (d >= (float) NDist) d -= 2*NDist;
      while (d < 0) d += 2*NDist;
    } // for iAngle
  } // for iPoint
  
  // we have to provide a comparison between two "different" structures
  // (having different allocators is enough to make them unrelated)
  bool bSame = true;
  auto stl_begin = stl_image.cbegin();
  for (const auto& cm_map: cm_image) {
    // the std::equal() call compares pairs (int, int) of each map
    if (std::equal(cm_map.begin(), cm_map.end(), (stl_begin++)->begin()))
      continue;
    bSame = false;
    break;
  } // for
  
  BOOST_CHECK(bSame);
  
} // RunHoughTransformTreeTest()


//------------------------------------------------------------------------------
//--- registration of tests
//
// Boost needs now to know which tests we want to run.
// Tests are "automatically" registered, hence the BOOST_AUTO_TEST_CASE()
// macro name. The argument is a name for the test; each test will have a
// number of checks and it will fail if any of them does.
//

BOOST_AUTO_TEST_CASE(RunHoughTransformTree) {
  RunHoughTransformTreeTest();
  std::cout << "Done." << std::endl;
}
