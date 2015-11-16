# ======================================================================
#  lardata main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/lardata/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(lardata)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{ART_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  ALLOW_DEPRECATIONS
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcore v1_00_00 )
find_ups_product( nutools v1_00_00 )
find_ups_product( postgresql v9_1_5 )
find_ups_product( cetbuildtools v4_09_00 )

#  Find all the libraries needed by our dependent CMakeList.txt files
cet_find_library( SIMULATIONBASE      NAMES SimulationBase      PATHS ENV NUTOOLS_LIB NO_DEFAULT_PATH )
cet_find_library( PQ                  NAMES pq                  PATHS ENV POSTGRESQL_LIBRARIES NO_DEFAULT_PATH )

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(AnalysisAlg)
add_subdirectory(AnalysisBase)
## retired - obsolete code for ArgoNeut - needs an overhaul for art 1.08.xx
##add_subdirectory(Monitoring)
add_subdirectory(OpticalDetectorData)
add_subdirectory(RawData)
add_subdirectory(RecoBase)
add_subdirectory(RecoBaseArt) # art-dependent interface to RecoBase objects
add_subdirectory(RecoObjects)
add_subdirectory(Utilities)
add_subdirectory(MCBase)
add_subdirectory(DataProviders)

# tests
add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
