/**
 * @file   larproperties_unit_test_base.h
 * @brief  Base class for objects initializing LArProperties
 * @date   December 1st, 2015
 * @author petrillo@fnal.gov
 * 
 * Provides an environment for easy set up of a LArProperties-aware test.
 * Keep in mind that, as much as I could push on flexibility, the implementation
 * of LArProperties must be hard-coded.
 * 
 * For an example of usage, see lardata/test/DetectorInfo/LArProperties_test.cc
 * 
 * Currently provides:
 * - BasicLArPropertiesEnvironmentConfiguration: test environment configuration
 * - LArPropertiesTesterEnvironment: a prepacked LArProperties-aware test
 *   environment
 * 
 */


#ifndef TEST_LARPROPERTIES_UNIT_TEST_BASE_H
#define TEST_LARPROPERTIES_UNIT_TEST_BASE_H

// LArSoft libraries
#include "lardata/DetectorInfo/LArProperties.h"
#include "test/Geometry/unit_test_base.h"

// utility libraries
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>


namespace testing {
  
  /** **************************************************************************
   * @brief Class holding a configuration for a test environment
   * @tparam PROVIDERIMPL class used for LArProperties provider implementation
   * @see LArPropertiesTesterEnvironment
   *
   * This class needs to be fully constructed by the default constructor
   * in order to be useful as Boost unit test fixture.
   * It is supposed to be passed as a template parameter to another class
   * that can store an instance of it and extract configuration information
   * from it.
   */
  template <typename PROVIDERIMPL>
  struct BasicLArPropertiesEnvironmentConfiguration:
    public BasicEnvironmentConfiguration
  {
    
    using LArProperties_t = PROVIDERIMPL;
    
    /// Default constructor; this is what is used in Boost unit test
    BasicLArPropertiesEnvironmentConfiguration():
      BasicEnvironmentConfiguration()
      { LocalInit(); }
    
    /// Constructor: acquires parameters from the command line
    BasicLArPropertiesEnvironmentConfiguration(int argc, char** argv):
      BasicEnvironmentConfiguration(argc, argv)
      { LocalInit(); }
    
    /// Constructor; accepts the name as parameter
    BasicLArPropertiesEnvironmentConfiguration(std::string name):
      BasicEnvironmentConfiguration(name)
      { LocalInit(); }
    
    BasicLArPropertiesEnvironmentConfiguration
      (int argc, char** argv, std::string name):
      BasicEnvironmentConfiguration(argc, argv, name)
      { LocalInit(); }
    
    /// @{
    /// @name Access to configuration
    /// FHiCL path for LArProperties configuration
    std::string LArPropertiesParameterSetPath() const
      { return ServiceParameterSetPath(LArPropertiesServiceName()); }
    
    /// A string describing the default parameter set to configure LArProperties
    std::string DefaultLArPropertiesConfiguration() const
      { return DefaultServiceConfiguration(LArPropertiesServiceName()); }
    ///@}
    
    
    /// @{
    /// @name Set configuration
    /// Sets the FHiCL path for LArProperties configuration
    void SetLArPropertiesParameterSetPath(std::string path)
      { SetServiceParameterSetPath(LArPropertiesServiceName(), path); }
    
    ///@}
    
    
    /// Returns the name of the service interface
    static std::string LArPropertiesServiceName()
      { return "LArPropertiesService"; }
    
      protected:
    
    /// Initialize with some default values
    void LocalInit()
      {
        // I am giving up: no valid LArProperties default configuration here!
      } // LocalInit()
    
  }; // class BasicLArPropertiesEnvironmentConfiguration<>
  
  
  
  /** **************************************************************************
   * @brief Environment for a LArProperties test
   * @tparam ConfigurationClass a class providing compile-time configuration
   * @see TesterEnvironment
   * 
   * The test environment is set up on construction.
   * 
   * The environment provides:
   * - LArProperties() method to access LArProperties (as a constant pointer)
   * - ... and everything TesterEnvironment provides
   * 
   * - Parameters() method returning the complete FHiCL configuration
   * - TesterParameters() method returning the configuration for the test
   * 
   * This class or a derived one can be used as global fixture for unit tests
   * that require the presence of LArProperties (in the form of
   * detinfo::LArProperties instance).
   * 
   * Unfortunately Boost does not give any control on the initialization of the
   * object, so everything must be ready to go as hard coded.
   * The ConfigurationClass class tries to alleviate that.
   * That is another, small static class that LArPropertiesTesterEnvironment
   * uses to get its parameters.
   * 
   * The requirements for the ConfigurationClass are:
   * - `LArProperties_t`: concrete type of LArProperties class
   * - `std::string ApplicationName()`: the application name
   * - `std::string ConfigurationPath()`: path to the configuration file
   * - `std::string LArPropertiesParameterSetPath()`: FHiCL path to the
   *   configuration of LArProperties; in art is
   *   `"services.LArPropertiesService"`
   * - `std::string TesterParameterSetPath()`: FHiCL path to the configuration
   *   of the specified test
   * - `std::string DefaultLArPropertiesConfiguration()` returning a FHiCL
   *   string to be parsed to extract the default LArProperties configuration
   *   (if such a thing exists)
   * - `std::string DefaultTesterConfiguration()` returning a FHiCL string
   *   to be parsed to extract the default test configuration
   * 
   * Whether the configuration comes from a file or from the two provided
   * defaults, it is always expected within the parameter set paths:
   * the default configuration must also contain that path.
   * 
   * Note that there is no room for polymorphism here since the setup happens
   * on construction.
   * Some methods are declared virtual in order to allow to tweak some steps
   * of the set up, but it's not trivial to create a derived class that works
   * correctly: the derived class must declare a new default constructor,
   * and that default constructor must call the protected constructor
   * (LArPropertiesTesterEnvironment<ConfigurationClass>(no_setup))
   */
  template <typename ConfigurationClass>
  class LArPropertiesTesterEnvironment:
    virtual public TesterEnvironment<ConfigurationClass>
  {
    /// Base class
    using TesterEnvironment_t = TesterEnvironment<ConfigurationClass>;
    
    /// this implements the singleton interface
    using LArpResources_t
      = TestSharedGlobalResource<detinfo::LArProperties const>;
    
      public:
    using SharedLArpPtr_t = LArpResources_t::ResourcePtr_t;
    
    /**
     * @brief Constructor: sets everything up and declares the test started
     * 
     * The configuration is from a default-constructed ConfigurationClass.
     * This is suitable for use as Boost unit test fixture.
     */
    LArPropertiesTesterEnvironment(bool bSetup = true):
      TesterEnvironment_t(false)
      { if (bSetup) Setup(); }
    
    //@{
    /**
     * @brief Setup from a configuration
     * @param configurer an instance of ConfigurationClass
     * 
     * The configuration is from the specified configurer class.
     * 
     * This constructor allows to use a non-default-constructed configuration.
     * This can't be used (at best of my knowledge) when using this class as
     * Boost unit test fixture.
     * 
     * In the r-value-reference constructor, the configurer is moved.
     */
    LArPropertiesTesterEnvironment
      (ConfigurationClass const& cfg_obj, bool bSetup = true):
      TesterEnvironment_t(cfg_obj, false)
      { if (bSetup) Setup(); }
    LArPropertiesTesterEnvironment
      (ConfigurationClass&& cfg_obj, bool bSetup = true):
      TesterEnvironment_t(std::move(cfg_obj), false)
      { if (bSetup) Setup(); }
    //@}
    
    /// Destructor: closing remarks
    virtual ~LArPropertiesTesterEnvironment();
    
    
    //@{
    /// Returns a pointer to LArProperties
    detinfo::LArProperties const* LArProperties() const { return larp.get(); }
    SharedLArpPtr_t SharedLArProperties() const { return larp; }
    //@}
    
    
    /// Returns the current global LArProperties instance
    /// @throws std::out_of_range if not present
    static detinfo::LArProperties const* GlobalLArProperties()
      { return &LArpResources_t::Resource(); }
    
    /// Returns the current global LArProperties instance (nullptr if none)
    static SharedLArpPtr_t SharedGlobalLArProperties()
      { return LArpResources_t::ShareResource(); }
    
    
      protected:
    
    using LArProperties_t = typename ConfigurationClass::LArProperties_t;
    
    using TesterEnvironment_t::Config;
    
    /// The complete initialization, ran at construction by default
    virtual void Setup();
    
    /// Creates a new LArProperties
    virtual SharedLArpPtr_t CreateNewLArProperties() const;
    
    //@{
    /// Get ownership of the specified LArProperties and registers it as global
    virtual void RegisterLArProperties(SharedLArpPtr_t new_larp);
    virtual void RegisterLArProperties(detinfo::LArProperties const* new_larp)
      { RegisterLArProperties(SharedLArpPtr_t(new_larp)); }
    //@}
    
    /// Sets up LArProperties (creates and registers it)
    virtual void SetupLArProperties();
    
      private:
    
    SharedLArpPtr_t larp; ///< pointer to LArProperties provider
    
  }; // class LArPropertiesTesterEnvironment<>
  
  
  
  //****************************************************************************
  template <typename ConfigurationClass>
  LArPropertiesTesterEnvironment<ConfigurationClass>::~LArPropertiesTesterEnvironment()
  {
    
    mf::LogInfo("Test")
      << Config().ApplicationName() << " LArProperties completed.";
    
  } // LArPropertiesTesterEnvironment<>::~LArPropertiesTesterEnvironment()
  
  
  /** **************************************************************************
   * @brief Sets LArProperties up
   * 
   * This function sets up LArProperties according to the provided information.
   */
  template <typename ConfigurationClass>
  typename LArPropertiesTesterEnvironment<ConfigurationClass>::SharedLArpPtr_t
  LArPropertiesTesterEnvironment<ConfigurationClass>::CreateNewLArProperties
    () const
  {
    
    std::string ProviderParameterSetPath
      = Config().LArPropertiesParameterSetPath();
    
    //
    // create the new LArProperties service provider
    //
    fhicl::ParameterSet ProviderConfig
      = this->Parameters().template get<fhicl::ParameterSet>
        (ProviderParameterSetPath);
    detinfo::LArProperties* new_larp = new LArProperties_t(ProviderConfig);
    
    return SharedLArpPtr_t(new_larp);
  } // LArPropertiesTesterEnvironment<>::CreateNewLArProperties()
  
  
  template <typename ConfigurationClass>
  void LArPropertiesTesterEnvironment<ConfigurationClass>::RegisterLArProperties
    (SharedLArpPtr_t new_larp)
  {
    // update the current LArProperties, that becomes owner;
    // also update the global one if it happens to be already our previous
    // (in this case, it becomes co-owner)
    SharedLArpPtr_t my_old_larp = larp;
    larp = new_larp;
    // if the global service is already the one we register, don't bother
    if (SharedGlobalLArProperties() != new_larp)
      LArpResources_t::ReplaceDefaultSharedResource(my_old_larp, new_larp);
  } // LArPropertiesTesterEnvironment<>::RegisterLArProperties()
  
  
  
  template <typename ConfigurationClass>
  void LArPropertiesTesterEnvironment<ConfigurationClass>::SetupLArProperties()
  {
    RegisterLArProperties(CreateNewLArProperties());
  } // LArPropertiesTesterEnvironment<>::SetupLArProperties()
  
  
  template <typename ConfigurationClass>
  void LArPropertiesTesterEnvironment<ConfigurationClass>::Setup() {
    
    TesterEnvironment_t::Setup();
    
    //
    // set up LArProperties service
    //
    SetupLArProperties();
    
    
    mf::LogInfo("Test") << Config().ApplicationName()
      << " LArProperties setup complete.";
    
  } // LArPropertiesTesterEnvironment<>::Setup()
  
  
} // namespace testing

#endif // TEST_LARPROPERTIES_UNIT_TEST_BASE_H
