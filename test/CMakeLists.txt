# ======================================================================
#
# Testing
#
# ======================================================================

include(CetTest)
cet_enable_asserts()


cet_test( sparse_vector_test )

cet_test(raw_test USE_BOOST_UNIT
  LIBRARIES RawData
  )

add_subdirectory( RecoObjects )

# various integration tests

# these tests run a FCL file and fail only if lar exits with a bad exit code;
# they are fast and always run.
# FCL files need to be copied to the test area (DATAFILES directive) since they
# are not installed.
# OPTIONAL_GROUPS is set to execute the test in the "BROKEN" test suite;
# that is because this test is currently failing for MicroBooNE
cet_test(donothing HANDBUILT
  DATAFILES test_donothing.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./test_donothing.fcl
)


