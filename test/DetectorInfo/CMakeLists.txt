cet_test( ProviderPack_test USE_BOOST_UNIT )

cet_test( LArPropertiesStandard_test
  LIBRARIES
    DetectorInfo
  DATAFILES lartest_standard.fcl
  TEST_ARGS ./lartest_standard.fcl
)

cet_test( LArPropertiesLArTPCdetector_test
  HANDBUILT
  DATAFILES lartest_lartpcdetector.fcl
  TEST_EXEC LArPropertiesStandard_test
  TEST_ARGS ./lartest_lartpcdetector.fcl
)

cet_test( LArPropertiesBo_test
  HANDBUILT
  DATAFILES lartest_bo.fcl
  TEST_EXEC LArPropertiesStandard_test
  TEST_ARGS ./lartest_bo.fcl
)


install_headers()
install_fhicl()
install_source()
