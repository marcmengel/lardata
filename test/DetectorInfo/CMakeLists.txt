cet_test( LArPropertiesStandard_test
  LIBRARIES
    lardata_DetectorInfo
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


cet_test( DetectorClocksStandard_test
  LIBRARIES
    lardata_DetectorInfo
  DATAFILES clockstest_standard.fcl
  TEST_ARGS ./clockstest_standard.fcl
)

cet_test( DetectorClocksLArTPCdetector_test
  HANDBUILT
  DATAFILES clockstest_lartpcdetector.fcl
  TEST_EXEC DetectorClocksStandard_test
  TEST_ARGS ./clockstest_lartpcdetector.fcl
)

cet_test( DetectorClocksBo_test
  HANDBUILT
  DATAFILES clockstest_bo.fcl
  TEST_EXEC DetectorClocksStandard_test
  TEST_ARGS ./clockstest_bo.fcl
)

cet_test( DetectorClocksCSU40L_test
  HANDBUILT
  DATAFILES clockstest_csu40l.fcl
  TEST_EXEC DetectorClocksStandard_test
  TEST_ARGS ./clockstest_csu40l.fcl
)

cet_make_exec( DetectorPropertiesStandard_test
  LIBRARIES
    lardata_DetectorInfo
    NO_INSTALL
)

cet_test( DetectorPropertiesLArTPCdetector_test
  HANDBUILT
  DATAFILES dettest_lartpcdetector.fcl
  TEST_EXEC DetectorPropertiesStandard_test
  TEST_ARGS ./dettest_lartpcdetector.fcl
)

cet_test( DetectorPropertiesBo_test
  HANDBUILT
  DATAFILES dettest_bo.fcl
  TEST_EXEC DetectorPropertiesStandard_test
  TEST_ARGS ./dettest_bo.fcl
)

