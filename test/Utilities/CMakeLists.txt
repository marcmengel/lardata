
# BulkAllocator_test tests a pure header library (it's a template)
cet_test(BulkAllocator_test USE_BOOST_UNIT)

# run a FHiCL file with only ComputePi inside
cet_test(timingreference_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config timing_reference_test.fcl
  DATAFILES timing_reference_test.fcl
)

install_headers()
install_fhicl()
install_source()
