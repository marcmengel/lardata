
# BulkAllocator_test, NestedIterator_test and CountersMap_test test pure header
# libraries (they are templates)
cet_test(BulkAllocator_test USE_BOOST_UNIT)
cet_test(NestedIterator_test USE_BOOST_UNIT)
cet_test(CountersMap_test USE_BOOST_UNIT)

# run a FHiCL file with only ComputePi inside
cet_test(timingreference_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config timingreference_test.fcl
  DATAFILES timingreference_test.fcl
)

install_headers()
install_fhicl()
install_source()
