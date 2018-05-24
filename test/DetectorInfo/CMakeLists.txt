
cet_test( DetectorPropertiesBo_test
  HANDBUILT
  DATAFILES dettest_bo.fcl
  TEST_EXEC DetectorPropertiesStandard_test
  TEST_ARGS ./dettest_bo.fcl
)

install_source()
install_fhicl()
