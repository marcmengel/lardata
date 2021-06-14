# ======================================================================
#  lardata main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/lardata/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required (VERSION 3.9 FATAL_ERROR)

project(lardata  VERSION 09.02.04 LANGUAGES CXX)

find_package(cetmodules REQUIRED)
find_package(canvas_root_io)
find_package(art)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -I $ENV{CLANG_FQ_DIR}/include/c++/v1
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( nusimdata )
find_ups_product( larcoreobj )
find_ups_product( larcorealg )
find_ups_product( lardataobj )
find_ups_product( lardataalg )
find_ups_product( larcore  )
find_ups_product( art  )
find_ups_product(art_root_io)
find_ups_product( fftw )
find_package( postgresql  )
find_package( Range-v3 )

#  Find all the libraries needed by our dependent CMakeList.txt files
# cet_find_library( NUSIMDATA_SIMULATIONBASE NAMES nusimdata_SimulationBase PATHS ENV NUSIMDATA_LIB NO_DEFAULT_PATH )
cet_find_library( PQ  NAMES pq PATHS ENV POSTGRESQL_LIBRARIES )

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)
find_package(messagefacility)

# source
add_subdirectory(lardata)

# tests
add_subdirectory(test)
add_subdirectory(cmakeConfig)
# ups - table and config files
#add_subdirectory(ups)

# packaging utility
#include(UseCPack)
