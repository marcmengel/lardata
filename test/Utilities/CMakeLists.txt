# BulkAllocator_test, NestedIterator_test, CountersMap_test 
# and sparse_vector_test test pure header libraries (they are templates)
cet_test(BulkAllocator_test USE_BOOST_UNIT)
cet_test(NestedIterator_test USE_BOOST_UNIT)
cet_test(CountersMap_test USE_BOOST_UNIT)
cet_test(sparse_vector_test)
cet_test(StatCollector_test USE_BOOST_UNIT)
cet_test(FastMatrixMath_test USE_BOOST_UNIT)
cet_test(SimpleFits_test USE_BOOST_UNIT)
cet_test(Dereference_test USE_BOOST_UNIT)

# run a FHiCL file with only ComputePi inside
cet_test(timingreference_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config timingreference_test.fcl
  DATAFILES timingreference_test.fcl
  OPTIONAL_GROUPS ONLY_SLF
)

# ------------------------------------------------------------------------------
# ---  LArPropertiesServiceArgoNeuT service
# ---
cet_test( LArPropertiesServiceArgoNeuT_test
  HANDBUILT
  DATAFILES larservicetest_argoneut.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./larservicetest_argoneut.fcl
)
# ------------------------------------------------------------------------------
# ---  DetectorPropertiesServiceArgoNeuT service
# ---
cet_test( DetectorPropertiesServiceArgoNeuT_test
  HANDBUILT
  DATAFILES detectorpropertiesservicetest_argoneut.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./detectorpropertiesservicetest_argoneut.fcl
)
# ------------------------------------------------------------------------------


install_headers()
install_fhicl()
install_source()

