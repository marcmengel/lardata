art_make(
  MODULE_LIBRARIES
    Geometry
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    ${MF_MESSAGELOGGER}
    ${MF_UTILITIES}
    ${FHICLCPP}
    ${CETLIB}
)

cet_test(LarProp HANDBUILT
  DATAFILES lartest.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./lartest.fcl
  )

cet_test(ServicePackTest HANDBUILT
  DATAFILES servicepacktest.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./servicepacktest.fcl
  )

# ------------------------------------------------------------------------------
# ---  LArProperties service
# ---
cet_test( LArPropertiesServiceStandard_test
  HANDBUILT
  DATAFILES larservicetest_standard.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./larservicetest_standard.fcl
)

cet_test( LArPropertiesServiceLArTPCdetector_test
  HANDBUILT
  DATAFILES larservicetest_lartpcdetector.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./larservicetest_lartpcdetector.fcl
)

cet_test( LArPropertiesServiceBo_test
  HANDBUILT
  DATAFILES larservicetest_bo.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./larservicetest_bo.fcl
)

cet_test( LArPropertiesServiceArgoNeuT_test
  HANDBUILT
  DATAFILES larservicetest_argoneut.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./larservicetest_argoneut.fcl
)
# ------------------------------------------------------------------------------


install_headers()
install_fhicl()
install_source()

