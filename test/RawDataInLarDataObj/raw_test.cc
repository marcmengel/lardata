/**
 * @file    raw_test.cc
 * @brief   Tests the raw data compression routines
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    20140716
 * @version 1.0
 *
 * This test covers only no compression and Huffman compression.
 * If compresses a data set, uncompresses it back and checks that the result
 * is the same as the original one.
 * As such, it does not support lossy compression (like zero suppression).
 *
 * See http://www.boost.org/libs/test for the Boost test library home page.
 * 
 * Timing:
 * version 1.0 takes less than 3" on a 3 GHz machine
 */

// C/C++ standard libraries
#include <cmath> // std::sqrt()
#include <limits> // std::numeric_limits<>
#include <random> // std::default_random_engine, ...
#include <string>
#include <vector>
#include <map>
#include <iostream>

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
#define BOOST_TEST_MODULE ( raw_test )
#include "cetlib/quiet_unit_test.hpp" // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK()

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::Compress_t
#include "lardataobj/RawData/raw.h"


/// The seed for the default random engine
constexpr unsigned int RandomSeed = 12345;


//------------------------------------------------------------------------------
//---  Input data creation
//
// Here are some classes to produce input data for the tests.
// All the classes, derived from DataCreatorBase, need to override the create()
// method to provide their data.
// The base class provides by default a random engine (from C++ STL)
// with a fixed seed.
//

/// Interface of an object creating data to be tested on
class DataCreatorBase {
		public:
	typedef std::vector<short> InputData_t;
	
	/// Random engine shared by all the data creators
	static std::default_random_engine random_engine;
	
	/// Constructor: just assigns a name to this data set
	DataCreatorBase(std::string new_name): test_name(new_name) {}
	
	/// Returns the name of this set
	std::string name() const { return test_name; }
	
	/// Virtual destructor
	virtual ~DataCreatorBase() {}
	
	/// Creates and returns the data sample; pure virtual
	virtual InputData_t create(size_t size) = 0;
	
		private:
	std::string test_name; ///< internal storage for test name
}; // class DataCreatorBase

// Initialized with a hard-coded seed
std::default_random_engine DataCreatorBase::random_engine(RandomSeed);



/// Data creator: uniformly random data, specified range plus offset (pedestal)
class UniformNoiseCreator: public DataCreatorBase {
		public:
	float baseline; ///< pedestal
	float width; ///< (half)width of the uniform distribution
	
	/// Constructor: assigns data set name and noise parameters
	UniformNoiseCreator
		(std::string name, float RMS, float pedestal = 0.):
		DataCreatorBase(name),
		baseline(pedestal), width(RMS * std::sqrt(12.))
		{}
	
	/// Creates and returns the data sample
	virtual InputData_t create(size_t size) override
		{
			std::uniform_real_distribution<float> noise
				(baseline - width, baseline + width);
			InputData_t data(size);
			for (auto& item: data)
				item = InputData_t::size_type(noise(random_engine));
			return data;
		} // create()
	
}; // class UniformNoiseCreator


/// Data creator: Gaussian random data
class GaussianNoiseCreator: public DataCreatorBase {
		public:
	float mean; ///< mean of the noise Gaussian (pedestal)
	float stdev; ///< standard deviation of the noise Gaussian (RMS)
	
	/// Constructor: assigns data set name and noise parameters
	GaussianNoiseCreator
		(std::string name, float sigma, float mu = 0.):
		DataCreatorBase(name),
		mean(mu), stdev(sigma)
		{}
	
	/// Creates and returns the data sample
	virtual InputData_t create(size_t size) override
		{
			std::normal_distribution<float> noise(mean, stdev);
			InputData_t data(size);
			for (auto& item: data)
				item = InputData_t::size_type(noise(random_engine));
			return data;
		} // create()
	
}; // class GaussianNoiseCreator


/// Data creator: sine wave data
class SineWaveCreator: public DataCreatorBase {
		public:
	float period; ///< period of the wave [ticks]
	float amplitude; ///< amplitude of the wave [ADC counts]
	
	/// Constructor: assigns data set name and noise parameters
	SineWaveCreator
		(std::string name, float new_period, float new_amplitude):
		DataCreatorBase(name),
		period(new_period), amplitude(new_amplitude)
		{}
	
	/// Creates and returns the data sample
	virtual InputData_t create(size_t size) override
		{
			constexpr float two_pi = float(2. * std::acos(-1.));
			InputData_t data;
			data.reserve(size);
			for (size_t i = 0; i < size; ++i)
			  data.push_back(amplitude * std::sin(i / period * two_pi));
			return data;
		} // create()
	
}; // class SineWaveCreator


/// Data creator: uniformly random data, full range
class RandomDataCreator: public DataCreatorBase {
		public:
	
	/// Creates and returns the data sample
	virtual InputData_t create(size_t size) override
		{
			std::uniform_int_distribution<short> uniform(
				std::numeric_limits<short>::min(),
				std::numeric_limits<short>::max()
				);
			InputData_t data(size);
			for (auto& item: data)
				item = InputData_t::value_type(uniform(random_engine));
			return data;
		} // create()
	
}; // class RandomDataCreator


//------------------------------------------------------------------------------
//--- Test code
//
// The tests are going to be repetitive: do almost the same thing in slightly
// different ways over and over.
// For that reason we have a data generator with a generic interface, and also
// the following functions to perform tests repeatedly.
//

/**
 * @brief Tests a data set with a given compression
 * @param id string representing the test (short descriptive text)
 * @param data data set (a vector of short integers)
 * @param mode compression mode to be tested
 * @see RunDataCompressionTests
 *
 * The test consists in compressing and uncompressing back data.
 * The test fails if the uncompressed data does not match the original one.
 * 
 * This is actually a single step of the full test implemented in
 * RunDataCompressionTests.
 */
void RunDataCompressionTest
	(std::string id, const std::vector<short>& data, raw::Compress_t mode)
{
	// working a copy of the original data:
	std::vector<short> buffer(data);
	
	// compress
	raw::Compress(buffer, mode);
	std::cout << id << ": compressed data size: " << buffer.size() << std::endl;
	
	// decompress (on an already allocated buffer)
	std::vector<short> data_again(data.size());
	raw::Uncompress(buffer, data_again, mode);
	
	// Boost provides facilities to compare data and print if it does not match:
	BOOST_CHECK_EQUAL(data_again.size(), data.size());
	
	BOOST_CHECK_EQUAL_COLLECTIONS
	  (data.begin(), data.end(), data_again.begin(), data_again.end());
	
} // RunDataCompressionTest()


/**
 * @brief Runs data compression tests with selected compression modes
 * @param pDataCreator an object to create the input data set
 * @see RunDataCompressionTest()
 *
 * This function hard-codes some sets of parameters for the tests:
 * the same test in RunDataCompressionTest() is repeated with different data
 * size and different compression modes.
 * This comprises the full test.
 */
void RunDataCompressionTests(DataCreatorBase* pDataCreator) {
	
	// the compression modes we are going to use, with a human-readable label
	std::map<raw::Compress_t, std::string> CompressionModes;
	CompressionModes[raw::kNone] = "uncompressed";
	CompressionModes[raw::kHuffman] = "Huffman";
//	CompressionModes[raw::kZeroSuppression] = "zero suppression";
//	CompressionModes[raw::kZeroHuffman] = "zero suppression plus Huffman";
//	CompressionModes[raw::kDynamicDec] = "dynamic";
	
	// the data sizes we are going to try, with a human-confusing label
	std::map<size_t, std::string> DataSizes;
	DataSizes[64] = "small size";
	DataSizes[9600] = "medium size";
	DataSizes[1048576] = "large size";
	
	for (const auto size_info: DataSizes) {
		// create the original data:
		const std::vector<short> data(pDataCreator->create(size_info.first));
		
		// test the same data with different compression algorithms:
		for (const auto test_info: CompressionModes) {
			// a "nice" label to make output vaguely meaningful...
			std::string test_id = pDataCreator->name()
				+ " (" + size_info.second + " " + test_info.second + ")";
			
			// ... and run the test
			RunDataCompressionTest(test_id, data, test_info.first);
		} // for compression modes
	} // for data sizes
	
	// that's it; Boost keeps records of successes and failures
} // RunDataCompressionTests()


//------------------------------------------------------------------------------
//--- registration of tests
//
// Boost needs now to know which tests we want to run.
// We run many RunDataCompressionTests() tests with different input data.
// Tests are "automatically" registered, hence the BOOST_AUTO_TEST_CASE()
// macro name. The argument is the name of the test; each step will have a
// number of checks and it will fail if any of them does.
//

BOOST_AUTO_TEST_CASE(NullData) {
	UniformNoiseCreator InputData("null input data", 0.);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(ConstantData) {
	UniformNoiseCreator InputData("constant input data", 0., 41.);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(SmallUniformNoiseData) {
	UniformNoiseCreator InputData("uniform small noise", 5.);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(SmallUniformNoiseOffsetData) {
	UniformNoiseCreator InputData("uniform small noise and offset", 5., 123.2);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(LargeUniformNoiseData) {
	UniformNoiseCreator InputData("uniform large noise", 40.);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(SmallGaussianNoiseOffsetData) {
	GaussianNoiseCreator InputData("Gaussian small noise and offset", 5., 123.2);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(LargeGaussianNoiseData) {
	GaussianNoiseCreator InputData("Gaussian large noise and offset", 40., 194.);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(VeryLowFrequencySineWaveData) {
	SineWaveCreator InputData("Very low frequency pure sine wave", 1024., 50.);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(LowFrequencySineWaveData) {
	SineWaveCreator InputData("Low frequency pure sine wave", 128., 100.);
	RunDataCompressionTests(&InputData);
}

BOOST_AUTO_TEST_CASE(HighFrequencySineWaveData) {
	SineWaveCreator InputData("High frequency pure sine wave", 16., 100.);
	RunDataCompressionTests(&InputData);
}

